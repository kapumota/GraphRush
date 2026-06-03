from pathlib import Path

def main() -> None:
    output = Path("data/security/security_edges.csv")
    output.parent.mkdir(parents=True, exist_ok=True)

    rows = [
        ("source", "target", "relation", "weight"),
        ("user_15", "group_admin", "member_of", "1"),
        ("group_admin", "server_db", "has_access", "1"),
        ("machine_22", "user_15", "logged_in_as", "1"),
        ("ip_10_0_0_5", "machine_22", "connects_to", "1"),
        ("attacker_machine", "ip_10_0_0_5", "reaches", "1"),
        ("server_db", "critical_asset", "stores", "1"),
    ]

    with output.open("w", encoding="utf-8") as f:
        for row in rows:
            f.write(",".join(row) + "\n")

    print("[GraphRush] Grafo de seguridad generado correctamente.")

if __name__ == "__main__":
    main()
