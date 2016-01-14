namespace Patheye
{

class Map : Object
{
	private uint8[] argb;

	public int width { get; construct set; }
	public int height { get; construct set; }
	public uint8[] raw { get; private set; }

	public Map.with_argb (uint8[] raw, uint8[] argb, int width, int height)
		requires (argb.length == width * height * 4)
	{
		Object
			( width: width
			, height: height
			);

		this.raw = raw;
		this.argb = argb;
	}

	public uint8[] get_argb ()
	{
		return this.argb;
	}
}

}
