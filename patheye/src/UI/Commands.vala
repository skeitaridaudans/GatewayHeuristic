namespace Patheye.UI
{

class Commands : Gtk.Menu
{
	public Gtk.Application application { get; construct set; }
	public Gtk.Window parent_window { get; construct set; }

	public signal void file_opened (string filename, uint8[] contents);
	public signal Error? file_saved (string filename);

	public Commands (Gtk.Application application, Gtk.Window parent_window)
	{
		Object
			( application: application
			, parent_window: parent_window
			);

		var open = new Gtk.MenuItem.with_label ("Open");
		var save = new Gtk.MenuItem.with_label ("Save");
		var about = new Gtk.MenuItem.with_label ("About");
		var close = new Gtk.MenuItem.with_label ("Close");

		this.add (open);
		this.add (save);
		this.add (new Gtk.SeparatorMenuItem ());
		this.add (about);
		this.add (close);

		open.activate.connect (on_open);
		save.activate.connect (on_save);
		about.activate.connect (on_about);
		close.activate.connect (on_close);

		this.show_all ();
	}

	private void on_open ()
	{
		var dialog = new Gtk.FileChooserDialog
			( "Open File"
			, this.parent_window
			, Gtk.FileChooserAction.OPEN
			, "Cancel"
			, Gtk.ResponseType.CANCEL
			, "Open"
			, Gtk.ResponseType.ACCEPT
			, null
			);

		dialog.select_multiple = true;
		dialog.set_current_folder("../data/maps");

		int response = dialog.run ();

		if (response == Gtk.ResponseType.ACCEPT)
		{
			SList<string> filenames = dialog.get_filenames ();

			foreach (var filename in filenames)
			{
				uint8[] contents;
				var file = File.new_for_path (filename);

				dialog.destroy ();

				try
				{
					file.load_contents (null, out contents, null);
				}
				catch (Error e)
				{
					var ed = new Gtk.MessageDialog
						( this.parent_window
						, Gtk.DialogFlags.MODAL
					//	| Gtk.DialogFlags.USE_HEADER_BAR
						, Gtk.MessageType.ERROR
						, Gtk.ButtonsType.OK
						, e.message
						);

					ed.run ();
					ed.destroy ();
				}

				this.file_opened (filename, contents);
			}
		}
		else
		{
			dialog.destroy ();
		}
	}

	private void on_save ()
	{
		var dialog = new Gtk.FileChooserDialog
			( "Save File"
			, this.parent_window
			, Gtk.FileChooserAction.SAVE
			, "Cancel"
			, Gtk.ResponseType.CANCEL
			, "Save"
			, Gtk.ResponseType.ACCEPT
			, null
			);

		dialog.do_overwrite_confirmation = true;
		dialog.set_current_folder("../data/maps");

		int response = dialog.run ();

		if (response == Gtk.ResponseType.ACCEPT)
		{
			string filename = dialog.get_filename ();
			dialog.destroy ();

			try
			{
				Error? e = this.file_saved (filename);

				if (e != null)
					throw e;
			}
			catch (Error e)
			{
				var ed = new Gtk.MessageDialog
					( this.parent_window
					, Gtk.DialogFlags.MODAL
				//	| Gtk.DialogFlags.USE_HEADER_BAR
					, Gtk.MessageType.ERROR
					, Gtk.ButtonsType.OK
					, e.message
					);

				ed.run ();
				ed.destroy ();
			}
		}
		else
		{
			dialog.destroy ();
		}
	}

	private void on_about ()
	{
		var dialog = new AboutDialog (this.parent_window);
		dialog.run ();
		dialog.destroy ();
	}

	private void on_close ()
	{
		this.parent_window.destroy ();
	}
}

}
