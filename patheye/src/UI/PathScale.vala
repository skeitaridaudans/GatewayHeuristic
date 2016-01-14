namespace Patheye.UI
{

class PathScale : Gtk.Scale
{
	private Patheye.Path? _model_path;
	public Patheye.Path? model_path
	{
		get
		{
			return _model_path;
		}

		set
		{
			this._model_path = value;
			this.adjustment.value = 0;

			if (value != null)
			{
				this.sensitive = true;
				this.adjustment.upper = value.get_segments ().length + 1;
			}
		}
	}

	public int step_increment
	{
		get
		{
			return (int) this.adjustment.step_increment;
		}

		set
		{
			this.adjustment.page_increment = value;
			this.adjustment.step_increment = value;
		}
	}

	public PathScale ()
	{
		Object
			( adjustment: new Gtk.Adjustment (0, 0, 100, 10, 10, 1)
			, digits: 0
			, orientation: Gtk.Orientation.HORIZONTAL
			, sensitive: false
			);
	}
}

}
