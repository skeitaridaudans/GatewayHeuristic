namespace Patheye
{

class PathParser
{
	[CCode (cname = "texpath_parse")]
	public extern static void texpath_parse
		( uint8[] contents
		, out Segment[] segments
		, out Gdk.Point[] path
		)
		throws Error;

	public static Path parse (uint8[] contents)
		throws Error
	{
		Segment[] segments;
		Gdk.Point[] path;

		texpath_parse (contents, out segments, out path);

		return new Path (segments, path);
	}
}

}
