import requests
import json
import sys

PINCHTAB_URL = "http://127.0.0.1:9867"
head={"Authorization": 'Bearer {KEY}'.format(KEY="")}

def pinchtab_navigate(url):
    payload = {"url": url}
    try:
        url = f"{PINCHTAB_URL}/navigate"
        response = requests.post(url, json=payload,headers=head, timeout=15)
        if response.status_code == 200:
            return "Success"
        return f"Failed: {response.text}"
    except Exception as e:
        return f"Exception during pinchtab navigate: {str(e)}"

def pinchtab_click(element):
    payload = {"kind":"click","ref":element}
    try:
        url = f"{PINCHTAB_URL}/action"

        response = requests.post(url, json=payload,headers=head, timeout=15)
        if response.status_code == 200:
            return response.text
        return f"Failed: {response.text}"
    except Exception as e:
        return f"Exception during pinchtab click: {str(e)}"

def pinchtab_type(element,input_value=""):
    if input_value=="":
        return "please add the input_value parameter"
    payload={"kind":"type","ref":element,"text":input_value}
    try:
        url = f"{PINCHTAB_URL}/action"
        response = requests.post(url, json=payload,headers=head, timeout=15)
        if response.status_code == 200:
            return "Success"
        return f"Failed: {response.text}"
    except Exception as e:
        return f"Exception during pinchtab type: {str(e)}"

def tool_handler(raw_args_from_cpp):
    try:
        args = json.loads(raw_args_from_cpp)
        action = args.get("action")
        if action == "nav":
            return pinchtab_navigate(args.get("target"))
        elif action == "click":
            return pinchtab_click(args.get("target"))
        elif action == "type":
            return pinchtab_type(args.get("target"), args.get("input_value"))
        return "Unknown action"
    except Exception as e:
        return f"Error in tool_handler: {str(e)}"


if __name__ =="__main__":
    if len(sys.argv) > 1:
        print(tool_handler(sys.argv[1]))
    else:
        print(
            "Error: No arguments provided.")