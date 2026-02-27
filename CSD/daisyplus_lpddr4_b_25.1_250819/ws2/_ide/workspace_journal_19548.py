# 2025-09-17T15:04:22.283383700
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

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

