from pythonosc import udp_client

msg = 1
route = "/route"

client = udp_client.SimpleUDPClient("127.0.0.1", 7777)
client.send_message(route, msg)



