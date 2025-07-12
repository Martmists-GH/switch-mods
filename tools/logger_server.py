import asyncio
from datetime import datetime
import logging

from websockets import ConnectionClosedError
from websockets.asyncio.server import serve, ServerConnection

log_file = "socket.log"

logging.basicConfig(filename=log_file,
                    format="{message}",
                    style='{',
                    datefmt="%H:%M:%S",
                    level=logging.INFO)

if log_file is not None:
    logging.getLogger().addHandler(logging.StreamHandler())

async def handler(ws: ServerConnection):
    addr, port = ws.remote_address
    logging.info(f"{datetime.now().strftime("%Y-%m-%d %H:%M:%S")} [{addr}] Connected!")
    while True:
        try:
            msg = await ws.recv()
            logging.info(f"{datetime.now().strftime("%Y-%m-%d %H:%M:%S")} [{addr}] {msg}".strip())
        except ConnectionClosedError:
            logging.warning(f"{datetime.now().strftime("%Y-%m-%d %H:%M:%S")} [{addr}] Client disconnected.")
            return
async def main():
    async with serve(handler, "0.0.0.0", 3080) as server:
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
