# from src.simulator import PhysicsSimulator

# simulator = PhysicsSimulator()

# # Valid input
# result = simulator.simulate(
#     pressure=50,
#     flow=20,
#     rpm=1500
# )

# print("Valid Input:")
# print(result)

# # Invalid pressure
# result = simulator.simulate(
#     pressure=120,
#     flow=20,
#     rpm=1500
# )

# print("\nInvalid Pressure:")
# print(result)
from src.interface import PhysicsEngineInterface


engine = PhysicsEngineInterface()

command = {
    "pressure": 50,
    "flow": 20,
    "rpm": 1500
}

result = engine.process_command(command)

print("Simulation Result:")
print(result)