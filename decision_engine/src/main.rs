use serde::{Deserialize, Serialize};
use std::env;
use std::io::{self, Read};

#[derive(Debug, Serialize, Deserialize)]
struct PhysicalState {
    pump_rpm: f64,
    valve_opening: f64,
    flow_rate: f64,
    pressure: f64,
    safe: bool,
}

#[derive(Debug, Serialize)]
struct Decision {
    decision: String,
    reason: String,
}

#[derive(Debug, Serialize)]
struct DecisionResponse {
    state: PhysicalState,
    result: Decision,
}

fn evaluate(state: &PhysicalState) -> Decision {
    // Physical safety limits
    const MIN_PRESSURE: f64 = 10.0;
    const MAX_PRESSURE: f64 = 100.0;
    const MAX_PUMP_RPM: f64 = 3500.0;
    const MAX_VALVE_OPENING: f64 = 90.0;

    if state.pressure < MIN_PRESSURE {
        return Decision {
            decision: "BLOCK".to_string(),
            reason: format!(
                "Pressure too low: {:.2} bar",
                state.pressure
            ),
        };
    }

    if state.pressure > MAX_PRESSURE {
        return Decision {
            decision: "BLOCK".to_string(),
            reason: format!(
                "Pressure exceeds safe limit: {:.2} bar",
                state.pressure
            ),
        };
    }

    if state.pump_rpm > MAX_PUMP_RPM {
        return Decision {
            decision: "BLOCK".to_string(),
            reason: format!(
                "Pump RPM exceeds safe limit: {:.2}",
                state.pump_rpm
            ),
        };
    }

    if state.valve_opening > MAX_VALVE_OPENING {
        return Decision {
            decision: "BLOCK".to_string(),
            reason: format!(
                "Valve opening exceeds safe limit: {:.2}%",
                state.valve_opening
            ),
        };
    }

    if !state.safe {
        return Decision {
            decision: "BLOCK".to_string(),
            reason: "Physics engine reported an unsafe state".to_string(),
        };
    }

    Decision {
        decision: "ALLOW".to_string(),
        reason: "Command keeps the physical system within safe limits"
            .to_string(),
    }
}

fn run_json_mode() {
    let mut input = String::new();

    if let Err(error) = io::stdin().read_to_string(&mut input) {
        eprintln!("Failed to read JSON input: {}", error);
        return;
    }

    if input.trim().is_empty() {
        eprintln!("No JSON input received.");
        return;
    }

    let state: PhysicalState = match serde_json::from_str(&input) {
        Ok(value) => value,
        Err(error) => {
            eprintln!("Invalid JSON input: {}", error);
            return;
        }
    };

    let decision = evaluate(&state);

    let response = DecisionResponse {
        state,
        result: decision,
    };

    match serde_json::to_string_pretty(&response) {
        Ok(json) => println!("{}", json),
        Err(error) => eprintln!("Failed to create JSON output: {}", error),
    }
}

fn run_cli_mode(args: &[String]) {
    if args.len() != 6 {
        println!("VoltGuard Decision Engine");
        println!();
        println!("CLI Usage:");
        println!(
            "cargo run -- <pump_rpm> <valve_opening> <flow_rate> <pressure> <safe>"
        );
        println!();
        println!("Example:");
        println!("cargo run -- 1000 60 20 42.67 1");
        println!();
        println!("JSON mode:");
        println!("echo '{{...}}' | cargo run -- --json");

        return;
    }

    let pump_rpm: f64 = match args[1].parse() {
        Ok(value) => value,
        Err(_) => {
            eprintln!("Invalid pump RPM.");
            return;
        }
    };

    let valve_opening: f64 = match args[2].parse() {
        Ok(value) => value,
        Err(_) => {
            eprintln!("Invalid valve opening.");
            return;
        }
    };

    let flow_rate: f64 = match args[3].parse() {
        Ok(value) => value,
        Err(_) => {
            eprintln!("Invalid flow rate.");
            return;
        }
    };

    let pressure: f64 = match args[4].parse() {
        Ok(value) => value,
        Err(_) => {
            eprintln!("Invalid pressure.");
            return;
        }
    };

    let safe_flag: i32 = match args[5].parse() {
        Ok(value) => value,
        Err(_) => {
            eprintln!("Invalid safe flag.");
            return;
        }
    };

    let state = PhysicalState {
        pump_rpm,
        valve_opening,
        flow_rate,
        pressure,
        safe: safe_flag == 1,
    };

    let decision = evaluate(&state);

    println!("======================================");
    println!("        VOLTGUARD DECISION ENGINE");
    println!("======================================");

    println!("Pump RPM       : {:.2}", state.pump_rpm);
    println!("Valve Opening  : {:.2}%", state.valve_opening);
    println!("Flow Rate      : {:.2}", state.flow_rate);
    println!("Pressure       : {:.2} bar", state.pressure);
    println!("Physics Safe   : {}", state.safe);

    println!("--------------------------------------");

    println!("Decision       : {}", decision.decision);
    println!("Reason         : {}", decision.reason);

    println!("======================================");
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() >= 2 && args[1] == "--json" {
        run_json_mode();
        return;
    }

    run_cli_mode(&args);
}
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn safe_state_should_be_allowed() {
        let state = PhysicalState {
            pump_rpm: 1000.0,
            valve_opening: 60.0,
            flow_rate: 20.0,
            pressure: 42.67,
            safe: true,
        };

        let decision = evaluate(&state);

        assert_eq!(decision.decision, "ALLOW");
    }

    #[test]
    fn high_pressure_should_be_blocked() {
        let state = PhysicalState {
            pump_rpm: 4000.0,
            valve_opening: 20.0,
            flow_rate: 26.67,
            pressure: 114.67,
            safe: false,
        };

        let decision = evaluate(&state);

        assert_eq!(decision.decision, "BLOCK");
    }

    #[test]
    fn excessive_pump_rpm_should_be_blocked() {
        let state = PhysicalState {
            pump_rpm: 4000.0,
            valve_opening: 60.0,
            flow_rate: 53.33,
            pressure: 106.67,
            safe: false,
        };

        let decision = evaluate(&state);

        assert_eq!(decision.decision, "BLOCK");
    }

    #[test]
    fn unsafe_physics_state_should_be_blocked() {
        let state = PhysicalState {
            pump_rpm: 2000.0,
            valve_opening: 50.0,
            flow_rate: 33.33,
            pressure: 60.0,
            safe: false,
        };

        let decision = evaluate(&state);

        assert_eq!(decision.decision, "BLOCK");
    }
}