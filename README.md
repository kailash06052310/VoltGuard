# VoltGuard

## Physics-Aware Industrial OT Security System

VoltGuard is a security system for industrial Operational Technology (OT)
environments. It evaluates Modbus/TCP control commands against a simulated
physical plant before allowing or blocking them.

Unlike a network-only firewall, VoltGuard considers the predicted physical
effect of a command.

---

## 1. Problem Statement

Traditional network security can identify suspicious packets, but a
technically valid industrial command may still be dangerous to the physical
process.

For example:

- A Modbus/TCP packet may be syntactically valid.
- The command may request an extremely high pump speed.
- The resulting physical pressure may exceed the safe operating limit.

VoltGuard addresses this problem by combining:

**Network inspection + Physics simulation + Security decision + Enforcement**

---

## 2. Architecture

```text
                    Modbus/TCP Command
                           |
                           v
                 +---------------------+
                 | C++ Packet           |
                 | Interceptor          |
                 +----------+----------+
                            |
                     Pump RPM / Command
                            |
                            v
                 +---------------------+
                 | Python Physics       |
                 | Engine               |
                 +----------+----------+
                            |
                    Predicted plant state
                            |
                            v
                 +---------------------+
                 | Rust Decision        |
                 | Engine               |
                 +----------+----------+
                            |
                     ALLOW / BLOCK
                            |
                  +---------+---------+
                  |                   |
                  v                   v
              FORWARD               DROP
                  |                   |
                  +---------+---------+
                            |
                            v
                 +---------------------+
                 | Security Logger     |
                 | JSONL Events        |
                 +----------+----------+
                            |
                            v
                 +---------------------+
                 | Qt OT Dashboard     |
                 +---------------------+
