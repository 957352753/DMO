# 2025-09-17T13:43:12.450897600
import vitis

client = vitis.create_client()
client.set_workspace(path="ws2")

platform = client.get_component(name="daisyplus")
status = platform.build()

comp = client.get_component(name="ftl")
comp.build()

status = platform.build()

comp.build()

vitis.dispose()

