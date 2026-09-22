from openai import OpenAI
import sys
import time
from dotenv import load_dotenv
import os

def print_response(text:str , delay = 0.02):
    for char in text:
        sys.stdout.write(char)
        sys.stdout.flush()
        time.sleep(delay)
    print()

def main()->None:
    load_dotenv()

    client = OpenAI(
        api_key=os.getenv("GROQ_API_KEY"),
        base_url="https://api.groq.com/openai/v1"
    )

    messages = [
        {
            "role": "system",
            "content": "You are a helpful AI assistant."
        }
    ]

    print("AI Chatbot Started (Powered by Groq)!")
    print("Type 'exit' to quit.\n")

    while True:
        user_input = input("You: ")

        if user_input.lower() == "exit" or user_input.lower() == "quit":
            break

        if not user_input.strip():
            continue

        messages.append({"role": "user", "content": user_input})

        try:
            response = client.chat.completions.create(
                model="llama-3.3-70b-versatile",
                messages=messages,
                temperature=0.7,
                max_tokens=512
            )
        except Exception as e:
            print(f"Error: {e}")
            messages.pop()
            continue

        bot_reply = response.choices[0].message.content

        print_response(f"Groq: {bot_reply}")

        messages.append({
            "role": "assistant",
            "content": bot_reply
        })

if __name__ == "__main__":
    main()

