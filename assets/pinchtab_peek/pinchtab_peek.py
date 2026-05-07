import requests
import json
import sys
import os
PINCHTAB_URL = "http://127.0.0.1:9867"
head=""

def pinchtab_find(input_value=""):#only search on the current web tab
    if input_value=="":
        return "please add the input_value parameter for search"
    payload = {"query":input_value,"threshold":0.3,"topK":3}
    try:
        url = f"{PINCHTAB_URL}/tabs/<tabId>/find"
        response = requests.post(url, json=payload,headers=head, timeout=15)
        if response.status_code == 200:
            return response.text
        return f"Failed: {response.text}"
    except Exception as e:
        return f"Exception during pinchtab find: {str(e)}"

def pinchtab_snap():#only show the interactive elements
    try:
        url = f"{PINCHTAB_URL}/snapshot?filter=interactive"
        response = requests.get(url,headers=head, timeout=15)
        if response.status_code == 200:
            return response.text
        return f"Failed: {response.text}"
    except Exception as e:
        return f"Exception during pinchtab snap: {str(e)}"


def tool_handler(data:dict):
    action=data.get("action")
    if action == "snap":
        return pinchtab_snap()
    elif action == "find":
        return pinchtab_find(data.get("input"))
    return "Unknown action"
def parameter_parser(raw_args_from_cpp):
    args = json.loads(raw_args_from_cpp)
    action = args.get("action")
    return tool_handler(args)



if __name__ =="__main__":
    try:
        env_vars = os.getenv("PINCHTAB_KEY")
        head = {"Authorization": 'Bearer {KEY}'.format(KEY=env_vars)}
        if len(sys.argv) > 1:
            print(parameter_parser(sys.argv[1]))
        else:
            print("Error: No arguments provided.")
    except Exception as e:
        print(e)