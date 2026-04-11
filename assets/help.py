import sys
import json
import os


def get_skill_info(skill_name):
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    if skill_name == "all" or not skill_name:
        skills = [d for d in os.listdir(base_dir)
                  if os.path.isdir(os.path.join(base_dir, d)) and d not in [ 'venv', '__pycache__']]
        return {"available_skills": skills,
                "instruction": "add 'skill_name' key in the json payload of skill 'help' to see a Specific skill JSON schema."}
    target_dir = os.path.join(base_dir, skill_name)
    json_path = os.path.join(target_dir, f"{skill_name}.json")
    if os.path.exists(json_path):
        try:
            with open(json_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
                return {
                    "status": "success",
                    "skill": skill_name,
                    "definition": config
                }
        except Exception as e:
            return {"status": "error", "message": f"parsing JSON failed: {str(e)}"}
    else:
        return {"status": "error", "message": f"cant find skill '{skill_name}'"}

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(json.dumps({"error": "No payload provided"}))
        sys.exit(1)

    raw_payload = sys.argv[1]

    try:
        data = json.loads(raw_payload)
        target = data.get("skill_name", "") if isinstance(data, dict) else str(data)
    except:
        target = raw_payload

    result = get_skill_info(target.strip())
    print(json.dumps(result, ensure_ascii=False))