class IndustrialPlant:
    """
    Simplified industrial water-pipeline model.

    The model estimates flow and pressure based on
    pump speed and valve opening.
    """

    def __init__(self):
        self.pump_rpm = 1000.0
        self.valve_opening = 50.0

        # Safe operating limits
        self.min_pressure = 10.0
        self.max_pressure = 100.0

        self.pressure = 0.0
        self.flow_rate = 0.0

    def simulate(self, pump_rpm=None, valve_opening=None):
        """
        Simulate the plant for the given pump and valve settings.
        """

        if pump_rpm is not None:
            self.pump_rpm = float(pump_rpm)

        if valve_opening is not None:
            self.valve_opening = float(valve_opening)

        # Keep values within physically meaningful ranges
        self.pump_rpm = max(0.0, self.pump_rpm)
        self.valve_opening = max(0.0, min(100.0, self.valve_opening))

        # Simplified flow calculation
        self.flow_rate = (
            (self.pump_rpm / 3000.0)
            * (self.valve_opening / 100.0)
            * 100.0
        )

        # Simplified pressure calculation
        pressure_from_pump = (self.pump_rpm / 3000.0) * 80.0
        pressure_from_restriction = (1.0 - self.valve_opening / 100.0) * 40.0

        self.pressure = pressure_from_pump + pressure_from_restriction

        return self.get_state()

    def is_safe(self):
        """Return True when the simulated pressure is within limits."""

        return (
            self.min_pressure
            <= self.pressure
            <= self.max_pressure
        )

    def get_state(self):
        """Return the current plant state."""

        return {
            "pump_rpm": round(self.pump_rpm, 2),
            "valve_opening": round(self.valve_opening, 2),
            "flow_rate": round(self.flow_rate, 2),
            "pressure": round(self.pressure, 2),
            "safe": self.is_safe(),
        }