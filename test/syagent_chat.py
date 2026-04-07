import requests
import json
import sys


def send_message(message):
    url = "http://localhost:9999/chat"
    headers = {'Content-Type': 'application/json'}
    data = json.dumps({"message": message})
    print("-------------------\n")
    try:
        response = requests.post(url, headers=headers, data=data, timeout=100, stream=True)
        response.raise_for_status()
        # full_raw = ""
        # for chunk in response.iter_content(chunk_size=1024):
        #     if chunk:
        #         full_raw += chunk.decode('utf-8', errors='ignore')
        #
        # lines = full_raw.split('\n')
        # for line in lines:
        #     if line.strip():
        #         try:
        #             chunk_data = json.loads(line)
        #             msg = chunk_data.get('message', '')
        #             print(msg, end='', flush=True)
        #         except:
        #             pass
        full_chunk = ""
        for chunk in response.iter_content(chunk_size=1024, decode_unicode=True):
            if chunk:
                try:
                    chunk_data = json.loads(chunk)
                    message_value = chunk_data.get('message', '')
                    full_chunk += message_value
                    if chunk_data.get('end', '')==1:
                        print(full_chunk, end='', flush=True)
                        full_chunk=""
                except json.JSONDecodeError:
                    pass
        return ""

    except requests.exceptions.RequestException as e:
        return f"Request failed: {e}"
    print("-------------------\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python script.py <message>")
        sys.exit(1)

    user_input = ' '.join(sys.argv[1:])
    reply = send_message(user_input)
    print()


if __name__ == "__main__":
    main()