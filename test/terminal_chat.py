import requests
import json
import sys
import os
url = "http://localhost:9995/chat"

def send_message(message):
        headers = {'Content-Type': 'application/json'}
        data = json.dumps({"message": f"{message}\n(current user is at : {os.getcwd()})", "id": "Super user"})
        print("-------------------\n")
        try:
            response = requests.post(url, headers=headers, data=data, timeout=100, stream=True)
            response.raise_for_status()
            full_chunk = ""
            for chunk in response.iter_content(chunk_size=1024, decode_unicode=True):
                if chunk:
                    try:
                        chunk_data = json.loads(chunk)
                        message_value = chunk_data.get('message', '')
                        full_chunk += message_value
                        if chunk_data.get('end', '') == 1:
                            print(full_chunk, end='', flush=True)
                            full_chunk = ""
                    except json.JSONDecodeError:
                        pass
            print("-------------------\n\n")
            return "Succeed"
        except requests.exceptions.RequestException as e:
            print("-------------------\n\n")
            return f"Request failed: {e}"

def main():
    if len(sys.argv) < 2:
        print("Error: Parameter format isnt correct")
        sys.exit(1)
    user_input = ' '.join(sys.argv[1:])
    reply = send_message(user_input)
    print()

if __name__ == "__main__":
    main()
