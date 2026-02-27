# 2025-09-17T11:27:30.419387600
import vitis

client = vitis.create_client()
client.set_workspace(path="ws2")

platform = client.get_component(name="daisyplus")
status = platform.build()

vitis.dispose()

