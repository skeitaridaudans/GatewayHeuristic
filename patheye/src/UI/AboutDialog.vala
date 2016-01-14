namespace Patheye.UI
{

class AboutDialog : Gtk.AboutDialog
{
	public AboutDialog (Gtk.Window parent)
	{
		Object
			( modal: true
			, transient_for: parent
			);

		const string[] authors =
			{ "Gabríel Arthúr Pétursson <gabriel@system.is>"
			};

		this.authors = authors;
		this.program_name = "Patheye";
		this.copyright = "Copyright © 2015 Gabríel Arthúr Pétursson";
		this.license_type = Gtk.License.GPL_3_0;
	}
}

}
