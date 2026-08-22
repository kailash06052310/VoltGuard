from physics_engine.src.physics_model import PhysicsModel
from physics_engine.src.simulator import PhysicsSimulator


def test_physics_model():
    model = PhysicsModel(
        pressure=50,
        flow=20,
        rpm=1500
    )

    state = model.get_parameters()

    assert state["pressure"] == 50
    assert state["flow"] == 20
    assert state["rpm"] == 1500


def test_safe_simulation():
    simulator = PhysicsSimulator()

    result = simulator.simulate(
        pressure=50,
        flow=20,
        rpm=1500
    )

    assert result["status"] == "SAFE"


def test_unsafe_pressure():
    simulator = PhysicsSimulator()

    result = simulator.simulate(
        pressure=100,
        flow=20,
        rpm=1500
    )

    assert result["status"] == "UNSAFE"