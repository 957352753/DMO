# 2025-09-02T14:07:53.095716500
import vitis

client = vitis.create_client()
client.set_workspace(path="ws2")

platform = client.get_component(name="daisyplus")
status = platform.build()

status = platform.build()

comp = client.get_component(name="ftl")
comp.build()

status = platform.build()

comp.build()

vitis.dispose()

vitis.dispose()

