namespace Patheye
{

class Path
{
	private Segment[] segments;
	private Gdk.Point[] path;

	public Path (Segment[] segments, Gdk.Point[] path)
	{
		this.segments = segments;
		this.path = path;
	}

	public Segment[] get_segments ()
	{
		return this.segments;
	}

	public Gdk.Point[] get_path ()
	{
		return this.path;
	}
}

}
