from flask import Flask, request, jsonify
import collections

app = Flask(__name__)

# Storage: { round_id: [ {sender_id: 1, data: "hex..."} ] }
messages = collections.defaultdict(list)

@app.route('/reset', methods=['POST'])
def reset():
    messages.clear()
    return jsonify({"status": "cleared"})

@app.route('/broadcast', methods=['POST'])
def broadcast():
    """
    Receives a message from a player for a specific round.
    JSON Body: { "sender_id": int, "round_id": int, "data": string }
    """
    content = request.json
    round_id = content.get('round_id')
    sender_id = content.get('sender_id')
    data = content.get('data')

    print(f"[Relay] Received Round {round_id} from Player {sender_id} (Size: {len(data)})")
    
    # Store message
    messages[round_id].append({
        "sender_id": sender_id,
        "data": data
    })
    return jsonify({"status": "ok"})

@app.route('/poll', methods=['GET'])
def poll():
    """
    Checks if all players have submitted for this round.
    Query Params: round_id, expected_count (total players usually 3)
    """
    round_id = request.args.get('round_id')
    expected_count = int(request.args.get('expected_count', 3))

    round_msgs = messages[round_id]
    
    # Check if we have enough messages (everyone sent)
    if len(round_msgs) >= expected_count:
        # Return map of {sender_id: data}
        data_map = {str(m['sender_id']): m['data'] for m in round_msgs}
        print(f"[Relay] Poll for {round_id}: returning {len(data_map)} entries")
        for sid, data in data_map.items():
            print(f"  Player {sid}: {data[:40]}... (len={len(data)})")
        return jsonify({"ready": True, "data": data_map})
    
    return jsonify({"ready": False})

if __name__ == '__main__':
    print("--- MPC Relay Server Running on Port 5000 ---")
    app.run(host='0.0.0.0', port=5000)