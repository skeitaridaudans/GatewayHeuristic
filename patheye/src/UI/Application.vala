namespace Patheye.UI
{

class Application : Gtk.Application
{
	public Application ()
	{
		Object
			( application_id: "is.system.patheye"
			, flags:          ApplicationFlags.FLAGS_NONE
			);
	}

	protected override void activate ()
	{
		var window = new Window (this);

		window.show_all ();
	}
}

}
