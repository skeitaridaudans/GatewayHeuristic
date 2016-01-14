namespace Patheye
{

class GateNetwork
{
	public Gate[] gates;

	public GateNetwork ()
	{
		this.gates = new Gate[0];
	}

	public GateNetwork.with_gates (Gate[] gates)
	{
		this.gates = gates;
	}

	public void add_gate (Gate gate)
	{
		var ngates = new Gate[this.gates.length + 1];

		for (int i = 0; i < this.gates.length; ++i)
		{
			ngates[i] = { this.gates[i].start, this.gates[i].end };
		}

		ngates[ngates.length - 1] = { gate.start, gate.end };

		this.gates = ngates;
	}

	public void delete_gate (PointF p)
	{
		double cd = 12.0;
		Gate? closest = null;

		foreach (unowned Gate gate in this.gates)
		{
			double dist = gate.distance (p);

			if (dist <= cd)
			{
				cd = dist;
				closest = gate;
			}
		}

		if (closest != null)
		{
			var ngates = new Gate[this.gates.length - 1];

			int skip = 0;
			for (int i = 0; i < this.gates.length - 1; ++i)
			{
				if (skip == 0 && this.gates[i] == closest)
					++skip;

				ngates[i] = { this.gates[i + skip].start, this.gates[i + skip].end };
			}

			this.gates = ngates;
		}
	}
}

}
