import requests
import json
import sys


def send_message(message):
    url = "http://localhost:9995/chat"
    headers = {'Content-Type': 'application/json'}
    data = json.dumps({"message": message})
    try:
        response = requests.post(url, headers=headers, data=data, timeout=30, stream=True)
        response.raise_for_status()

        full_response = ""
        for chunk in response.iter_content(chunk_size=1024, decode_unicode=True):
            if chunk:
                try:
                    chunk_data = json.loads(chunk)
                    message_value = chunk_data.get('message', '')
                    full_response += message_value
                    print(message_value, end='', flush=True)
                except json.JSONDecodeError:
                    pass
        return full_response

    except requests.exceptions.RequestException as e:
        return f"Request failed: {e}"


def main():
    if len(sys.argv) < 2:
        print("Usage: python script.py <message>")
        sys.exit(1)

    user_input = ' '.join(sys.argv[1:])
    reply = send_message(user_input)
    print()  # 换行
    # 如果需要保存完整响应
    # print("Full response:", reply)


if __name__ == "__main__":
    main()