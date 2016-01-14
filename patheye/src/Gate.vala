namespace Patheye
{

struct Gate
{
	public Gdk.Point start;
	public Gdk.Point end;

	public Gate (Gdk.Point start)
	{
		this.start = start;
		this.end = start;
	}

	public Gate.with_end (Gdk.Point start, Gdk.Point end)
	{
		this.start = start;
		this.end = end;
	}

	public Gate invert (int height)
	{
		return Gate.with_end
			( { this.start.x, height - this.start.y - 1 }
			, { this.end.x, height - this.end.y - 1 }
			);
	}

	public double distance (PointF p)
	{
		PointF v = { (double) start.x, (double) start.y };
		PointF w = { (double) end.x, (double) end.y };

		var l2 = dist2 (v, w);

		if (l2 == 0.0)
			return Math.sqrt (dist2 (p, v));

		var t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;

		if (t < 0.0)
			return Math.sqrt (dist2 (p, v));

		if (t > 1.0)
			return Math.sqrt (dist2 (p, w));

		return Math.sqrt (dist2 (p, { v.x + t * (w.x - v.x), v.y + t * (w.y - v.y) }));
	}

	private double sqr (double x)
	{
		return x * x;
	}

	private double dist2 (PointF v, PointF w)
	{
		return sqr (v.x - w.x) + sqr (v.y - w.y);
	}
}

}
