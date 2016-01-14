namespace Patheye.UI
{

class Window : Gtk.ApplicationWindow
{
	private ControlBar control_bar;
	private PathScale path_scale;
	private Board board;

	private Gtk.Label coordinates;

	public Window (Gtk.Application application)
	{
		Object
			( application: application
			);

		this.control_bar = new ControlBar (application, this);
		this.control_bar.commands.file_opened.connect (on_file_opened);
		this.control_bar.commands.file_saved.connect (on_file_saved);
		this.control_bar.play.connect (on_play);
		this.control_bar.stop.connect (on_stop);
		this.control_bar.steps_changed.connect (on_steps_changed);
		this.set_titlebar (control_bar);

		var box = new Gtk.Box (Gtk.Orientation.VERTICAL, 0);
		this.add (box);

		this.path_scale = new PathScale ();
		this.path_scale.value_changed.connect (on_path_scale_value_changed);
		box.add (this.path_scale);

		box.add (new Gtk.Separator (Gtk.Orientation.HORIZONTAL));

		this.board = new Board ();
		this.board.pointer_left.connect (on_pointer_left);
		this.board.pointer_moved.connect (on_pointer_moved);
		this.board.expand = true;
		box.add (this.board);

		box.add (new Gtk.Separator (Gtk.Orientation.HORIZONTAL));

		var statusbar = new Gtk.Statusbar ();
		this.coordinates = new Gtk.Label ("");
		statusbar.pack_end (this.coordinates);
		box.add (statusbar);

		this.show_all ();
	}

	private void on_file_opened (string filename, uint8[] contents)
	{
		try
		{
			if (contents.length == 0)
				throw new Error
					( Quark.from_string ("empty-file")
					, 0
					, "File is empty"
					);

			switch (contents[0])
			{
				case 's':
					var path = PathParser.parse (contents);

					this.control_bar.model_path = path;
					this.path_scale.model_path = path;
					this.board.model_path = path;
					break;
				case 't':
					this.board.model_map = MapParser.parse (contents);
					break;
				case 0:
					this.board.model_network = GateNetworkParser.parse (contents);
					break;
				default:
					throw new Error
						( Quark.from_string ("unknown-format")
						, 0
						, "Unknown file format"
						);
			}
		}
		catch (Error e)
		{
			var ed = new Gtk.MessageDialog
				( this
				, Gtk.DialogFlags.MODAL
		//		| Gtk.DialogFlags.USE_HEADER_BAR
				, Gtk.MessageType.ERROR
				, Gtk.ButtonsType.OK
				, e.message
				);

			ed.run ();
			ed.destroy ();
		}
	}

	private Error? on_file_saved (string filename)
	{
		try
		{
			GateNetworkParser.save
				( filename
				, this.board.model_map
				, this.board.model_network
				);
		}
		catch (Error e)
		{
			return e;
		}

		return null;
	}

	private void on_play ()
	{
		Timeout.add (100, on_timeout);
	}

	private void on_stop ()
	{
	}

	private void on_steps_changed (int steps)
	{
		this.path_scale.step_increment = steps;
	}

	private void on_path_scale_value_changed ()
	{
		this.board.drawn_segments = (int) this.path_scale.adjustment.value;
	}

	private bool on_timeout ()
	{
		if (this.control_bar.playing == false)
			return false;

		this.path_scale.adjustment.value += this.path_scale.step_increment;

		if
			(  this.path_scale.adjustment.value + 1
			>= this.path_scale.adjustment.upper
			)
		{
			this.control_bar.playing = false;

			return false;
		}

		return true;
	}

	private void on_pointer_left ()
	{
		this.coordinates.label = "";
	}

	private void on_pointer_moved (int x, int y)
	{
		this.coordinates.label
			=   "X: " + x.to_string ()
			+ ", Y: " + y.to_string ();
	}
}

}
