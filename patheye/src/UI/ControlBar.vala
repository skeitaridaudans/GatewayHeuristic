namespace Patheye.UI
{

class ControlBar : Gtk.HeaderBar
{
	private Gtk.Button play_button;
	private Gtk.ComboBoxText step_combobox;

	private Gtk.Image play_image;
	private Gtk.Image pause_image;

	public Gtk.Application application { get; construct set; }
	public Commands commands { get; construct set; }
	public Gtk.Window window { get; construct set; }

	private bool _playing;
	public bool playing
	{
		get
		{
			return this._playing;
		}

		set
		{
			if (this._playing && ! value)
			{
				this.play_button.remove (this.pause_image);
				this.play_button.add (this.play_image);

				this.stop ();
			}
			else if ( ! this._playing && value)
			{
				this.play_button.remove (this.play_image);
				this.play_button.add (this.pause_image);

				this.play ();
			}

			this._playing = value;
		}
	}

	private Patheye.Path? _model_path;
	public Patheye.Path? model_path
	{
		get
		{
			return this._model_path;
		}

		set
		{
			this._model_path = value;
			this.set_sensitivity (value != null);

			if (this.playing)
			{
				this.on_play_activate (this.play_button);
			}
		}
	}

	public signal void play ();
	public signal void stop ();
	public signal void steps_changed (int steps);

	public ControlBar (Gtk.Application application, Gtk.Window window)
	{
		Object
			( title: "Patheye"
			, show_close_button: true
			, application: application
			, window: window
			);

		this.play_image = new Gtk.Image.from_icon_name
			( "media-playback-start-symbolic"
			, Gtk.IconSize.BUTTON
			);
		this.play_image.show ();

		this.pause_image = new Gtk.Image.from_icon_name
			( "media-playback-pause-symbolic"
			, Gtk.IconSize.BUTTON
			);
		this.pause_image.show ();

		this.commands = new Commands (this.application, this.window);

		var system_image = new Gtk.Image.from_icon_name
			( "emblem-system-symbolic"
			, Gtk.IconSize.BUTTON
			);

		var menu_button = new Gtk.MenuButton ();
		menu_button.add (system_image);
		menu_button.set_popup (commands);
		this.pack_end (menu_button);

		this.play_button = new Gtk.Button ();
		this.play_button.add (this.play_image);
		this.play_button.clicked.connect (on_play_activate);
		this.pack_start (this.play_button);

		this.step_combobox = new Gtk.ComboBoxText ();
		this.step_combobox.append ("1", "1 Step");
		this.step_combobox.append ("10", "10 Steps");
		this.step_combobox.append ("100", "100 Steps");
		this.step_combobox.append ("1000", "1.000 Steps");
		this.step_combobox.active_id = "10";
		this.step_combobox.changed.connect (on_steps_combobox_changed);
		this.pack_start (this.step_combobox);

		this.set_sensitivity (false);
	}

	private void set_sensitivity (bool sensitive)
	{
		this.play_button.sensitive = sensitive;
		this.step_combobox.sensitive = sensitive;
	}

	private void on_play_activate (Gtk.Button button)
	{
		this.playing = ! this.playing;
	}

	private void on_steps_combobox_changed (Gtk.ComboBox combobox)
	{
		this.steps_changed (int.parse (combobox.active_id));
	}
}

}
