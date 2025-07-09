import asyncio
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
    logging.info(f"[{addr}] Connected!")
    while True:
        try:
            msg = await ws.recv()
            if not msg:
                logging.warning(f"[{addr}] Client sent an empty message, assuming disconnect.")
                break
            logging.info(f"[{addr}] {msg}".strip())
        except ConnectionClosedError:
            break
async def main():
    async with serve(handler, "0.0.0.0", 3080) as server:
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
