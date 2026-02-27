# 2025-09-10T15:27:10.276230400
import vitis

client = vitis.create_client()
client.set_workspace(path="ws2")

platform = client.get_component(name="daisyplus")
status = platform.build()

comp = client.get_component(name="ftl")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()

