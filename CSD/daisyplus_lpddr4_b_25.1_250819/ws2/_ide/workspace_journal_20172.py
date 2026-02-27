# 2025-10-30T09:52:06.435786700
import vitis

client = vitis.create_client()
client.set_workspace(path="ws2")

platform = client.get_component(name="daisyplus")
status = platform.build()

comp = client.get_component(name="ftl")
comp.build()

vitis.dispose()

