from scapy.all import IP, TCP, Raw


def create_modbus_tcp_packet(
    pump_rpm: int,
    src_ip: str = "192.168.1.10",
    dst_ip: str = "192.168.1.20",
    src_port: int = 12345,
    dst_port: int = 502,
):
    """
    Create a Modbus/TCP packet containing a Write Single Register command.

    Register 1 is used as the simulated pump RPM register.
    """

    transaction_id = 1
    protocol_id = 0
    unit_id = 1
    function_code = 0x06
    register_address = 1

    # Modbus PDU:
    # Function Code + Register Address + Register Value
    pdu = (
        function_code.to_bytes(1, "big")
        + register_address.to_bytes(2, "big")
        + pump_rpm.to_bytes(2, "big")
    )

    # MBAP header:
    # Transaction ID + Protocol ID + Length + Unit ID
    length = len(pdu) + 1

    mbap = (
        transaction_id.to_bytes(2, "big")
        + protocol_id.to_bytes(2, "big")
        + length.to_bytes(2, "big")
        + unit_id.to_bytes(1, "big")
    )

    payload = mbap + pdu

    packet = (
        IP(src=src_ip, dst=dst_ip)
        / TCP(sport=src_port, dport=dst_port)
        / Raw(load=payload)
    )

    return packet


def show_packet(packet):
    """Display packet details and raw Modbus payload."""

    print("=== Modbus/TCP Packet ===")
    packet.show()

    if Raw in packet:
        print("\nRaw Payload:")
        print(bytes(packet[Raw].load).hex(" "))


if __name__ == "__main__":

    print("Creating normal Modbus/TCP packet...")
    normal_packet = create_modbus_tcp_packet(1000)
    show_packet(normal_packet)

    print("\nCreating dangerous Modbus/TCP packet...")
    dangerous_packet = create_modbus_tcp_packet(4000)
    show_packet(dangerous_packet)