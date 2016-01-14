namespace Patheye.UI
{

class Board : Gtk.DrawingArea
{
	private PointF last_mouse;
	private int[,] bitclosed;
	private int[,] bitopened;

	private PointF _center;
	public PointF center
	{
		get
		{
			return this._center;
		}

		set
		{
			this._center = value;
			this.redraw ();
		}
	}

	private Patheye.Map? _model_map;
	public Patheye.Map? model_map
	{
		get
		{
			return this._model_map;
		}

		set
		{
			this.edit_gate = null;
			this._model_map = value;
			this.compute_bitmap ();
			this.redraw ();

			if (value == null)
			{
				this.pointer_left ();
			}
			else
			{
				this.center = { value.width / 2.0, value.height / 2.0 };
				this.scale = 1.0;
			}
		}
	}

	private Patheye.GateNetwork _model_network;
	public Patheye.GateNetwork model_network
	{
		get
		{
			return this._model_network;
		}

		set
		{
			this.edit_gate = null;
			this._model_network = value;
			this.redraw ();
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
			this.edit_gate = null;
			this._model_path = value;
			this.compute_bitmap ();
			this.redraw ();
		}
	}

	private double _scale;
	private double scale
	{
		get
		{
			return this._scale;
		}

		set
		{
			this._scale = value;
			this.redraw ();
		}
	}

	private int _drawn_segments;
	public int drawn_segments
	{
		get
		{
			return this._drawn_segments;
		}

		set
		{
			this._drawn_segments = value;
			this.redraw ();
		}
	}

	private Gate? _edit_gate;
	public Gate? edit_gate
	{
		get
		{
			return this._edit_gate;
		}

		set
		{
			if (value != null)
				this._edit_gate = Gate.with_end (value.start, value.end);
			else
				this._edit_gate = null;

			this.redraw ();
		}
	}

	public signal void pointer_left ();
	public signal void pointer_moved (int x, int y);

	public Board ()
	{
		Object ();

		this._center = { 0.0, 0.0 };
		this._scale = 1.0f;

		this._model_network = new GateNetwork ();

		this.set_events
			( Gdk.EventMask.BUTTON1_MOTION_MASK
			| Gdk.EventMask.BUTTON3_MOTION_MASK
			| Gdk.EventMask.BUTTON_PRESS_MASK
			| Gdk.EventMask.ENTER_NOTIFY_MASK
			| Gdk.EventMask.LEAVE_NOTIFY_MASK
			| Gdk.EventMask.POINTER_MOTION_MASK
			| Gdk.EventMask.SCROLL_MASK
			);

		this.can_focus = true;
		this.has_focus = true;
		this.set_size_request (600, 600);
		this.button_press_event.connect (on_button_press_event);
		this.enter_notify_event.connect (on_enter_notify_event);
		this.leave_notify_event.connect (on_leave_notify_event);
		this.motion_notify_event.connect (on_motion_notify_event);
		this.scroll_event.connect (on_scroll_event);
		this.draw.connect (on_draw);
	}

	private void redraw ()
	{
		int width = this.get_allocated_width ();
		int height = this.get_allocated_height ();

		this.queue_draw_area (0, 0, width, height);
	}

	private void update_point (double x, double y)
	{
		this.last_mouse = { x, y };

		PointF p = this.invproject (this.last_mouse);

		this.pointer_moved ((int) p.x, (int) p.y);

		if (this.edit_gate != null)
		{
			this.edit_gate.end = { (int) p.x, (int) p.y };
			this.redraw ();
		}
	}

	private bool on_button_press_event (Gdk.EventButton ev)
	{
		if (this.model_map == null)
			return true;

		PointF p = this.invproject ({ ev.x, ev.y });

		if (ev.button == 3)
		{
			if (this.edit_gate == null)
			{
				this.edit_gate = Gate ({ (int) p.x, (int) p.y });
			}
			else
			{
				//this.model_network.add_gate (this.edit_gate.invert (this.model_map.height));
				this.model_network.add_gate (this.edit_gate);
				this.edit_gate = null;
			}
		}

		if (ev.button == 2)
		{
			this.model_network.delete_gate ({ p.x, this.model_map.height - p.y - 1 });
			this.redraw ();
		}

		return true;
	}

	private bool on_enter_notify_event (Gdk.EventCrossing ev)
	{
		if (this.model_map == null)
			return true;

		this.update_point (ev.x, ev.y);

		return true;
	}

	private bool on_leave_notify_event (Gdk.EventCrossing ev)
	{
		if (this.model_map == null)
			return true;

		this.pointer_left ();

		return true;
	}

	private bool on_motion_notify_event (Gdk.EventMotion ev)
	{
		if (this.model_map == null)
			return true;

		if ((ev.state & Gdk.ModifierType.BUTTON1_MASK) > 0)
		{
			PointF a = this.invproject ({ ev.x, ev.y });
			PointF b = this.invproject (this.last_mouse);

			PointF delta = { b.x - a.x, a.y - b.y };
			this.center = { this.center.x + delta.x, this.center.y + delta.y };
		}

		this.update_point (ev.x, ev.y);

		return true;
	}

	private bool on_scroll_event (Gdk.EventScroll ev)
	{
		switch (ev.direction)
		{
			case Gdk.ScrollDirection.DOWN:
				this.scale *= 0.9;
				break;
			case Gdk.ScrollDirection.UP:
				this.scale *= 1.1;
				break;
		}

		this.update_point (ev.x, ev.y);

		return true;
	}

	private Cairo.Matrix projection ()
		requires (this.model_map != null)
	{
		Cairo.Matrix mat = Cairo.Matrix.identity ();

		int width = this.get_allocated_width ();
		int height = this.get_allocated_height ();
		double aspect = (double) width / (double) height;

		/* normalized device coordinates to Cairo device coordinates */
		mat.scale (width, height);
		mat.scale (0.5, 0.5);
		mat.translate (1.0, 1.0);
		mat.scale (1.0, -1.0);

		/* aspect ratio */
		if (aspect >= 1.0)
			mat.scale (1.0, aspect);
		else
			mat.scale (1.0 / aspect, 1.0);

		mat.scale (this.scale, this.scale);
		mat.scale (2.0 / this.model_map.width, 2.0 / this.model_map.height);
		mat.translate (-this.center.x, -this.center.y);

		return mat;
	}

	private PointF invproject (PointF p)
	{
		double x = p.x;
		double y = p.y;

		var mat = this.projection ();
		mat.invert ();
		mat.transform_point (ref x, ref y);

		return { x, _model_map.height - y };
	}

	private void compute_bitmap ()
	{
		if (this.model_map == null || this.model_path == null)
			return;

		var segments = this.model_path.get_segments ();

		bitclosed = new int[this.model_map.height, this.model_map.width];
		bitopened = new int[this.model_map.height, this.model_map.width];

		for (int i = 0; i < segments.length; ++i)
		{
			var segment = segments[i];

			foreach (Gdk.Point closed in segment.closed)
			{
				if (closed.x >= this.model_map.width || closed.y >= this.model_map.height)
					continue;

				if (bitclosed[closed.y, closed.x] == 0)
					bitclosed[closed.y, closed.x] = i + 1;
			}

			if
				(  segment.opened.x >= this.model_map.width
				|| segment.opened.y >= this.model_map.height
				)
				continue;

			if (bitopened[segment.opened.y, segment.opened.x] == 0)
				bitopened[segment.opened.y, segment.opened.x] = i + 1;
		}
	}

	private bool on_draw (Gtk.Widget widget, Cairo.Context ctx)
	{
		if (this.model_map == null)
			return true;

		var mat = this.projection ();

		mat.multiply (mat, ctx.get_matrix ());
		ctx.set_matrix (mat);
		ctx.set_line_cap (Cairo.LineCap.SQUARE);
		ctx.set_line_width (1.0);

		var map_surface = new Cairo.ImageSurface.for_data
			( this.model_map.get_argb ()
			, Cairo.Format.ARGB32
			, this.model_map.width
			, this.model_map.height
			, this.model_map.width * 4
			);

		ctx.set_source_surface (map_surface, 0.0, 0.0);
		ctx.get_source ().set_filter (Cairo.Filter.NEAREST);
		ctx.paint ();

		if (this.model_path != null)
		{
			/* draw segments */
			this.draw_segments (ctx);

			/* draw path */
			ctx.save ();
			ctx.scale (1.0, -1.0);
			ctx.translate (0.0, -this.model_map.height);

			ctx.set_source_rgb (0.4, 0.4, 1.0);

			foreach (Gdk.Point point in this.model_path.get_path ())
			{
				ctx.rectangle (point.x, point.y, 1, 1);
			}

			ctx.fill ();
			ctx.restore ();
		}

		/* draw gates */
		ctx.set_source_rgb (0.8, 0.0, 0.0);

		ctx.save ();
		ctx.scale (1.0, -1.0);
		ctx.translate (0.0, -this.model_map.height);

		foreach (unowned Gate gate in this.model_network.gates)
		{
			if (gate.start == gate.end)
			{
				ctx.stroke ();
				ctx.rectangle (gate.start.x, gate.start.y, 1, 1);
				ctx.fill ();
			}
			else
			{
				ctx.move_to (gate.start.x + 0.5, gate.start.y + 0.5);
				ctx.line_to (gate.end.x + 0.5, gate.end.y + 0.5);
			}
		}

		ctx.stroke ();
		ctx.restore ();


		/* draw edit gate */
		if (this.edit_gate != null)
		{
			ctx.save ();
			ctx.scale (1.0, -1.0);
			ctx.translate (0.0, -this.model_map.height);

			ctx.set_source_rgb (1.0, 0.0, 0.0);
			unowned Gate gate = this.edit_gate;

			if (gate.start == gate.end)
			{
				ctx.rectangle (gate.start.x, gate.start.y, 1, 1);
				ctx.fill ();
			}
			else
			{
				ctx.move_to (gate.start.x + 0.5, gate.start.y + 0.5);
				ctx.line_to (gate.end.x + 0.5, gate.end.y + 0.5);
				ctx.stroke ();
			}

			ctx.restore ();
		}

		return true;
	}

	private void draw_segments (Cairo.Context ctx)
	{
		var segments = this.model_path.get_segments ();

		ctx.save ();
		ctx.scale (1.0, -1.0);
		ctx.translate (0.0, -this.model_map.height);

		/* draw closed segments */
		Color[] closed_color = new Color[8];
		closed_color[0] = { 0.647, 0.839, 0.655 };
		closed_color[1] = { 0.506, 0.780, 0.518 };
		closed_color[2] = { 0.400, 0.733, 0.416 };
		closed_color[3] = { 0.298, 0.686, 0.314 };
		closed_color[4] = { 0.263, 0.627, 0.278 };
		closed_color[5] = { 0.220, 0.557, 0.235 };
		closed_color[6] = { 0.180, 0.490, 0.196 };
		closed_color[7] = { 0.106, 0.369, 0.125 };

		this.draw_color (ctx, closed_color[0]);
		this.draw_streak (ctx, this.bitclosed);

		/* draw opened segments */
		Color[] opened_color = new Color[8];
		opened_color[0] = { 0.690, 0.745, 0.773 };
		opened_color[1] = { 0.565, 0.643, 0.682 };
		opened_color[2] = { 0.471, 0.565, 0.612 };
		opened_color[3] = { 0.376, 0.490, 0.545 };
		opened_color[4] = { 0.329, 0.431, 0.478 };
		opened_color[5] = { 0.271, 0.353, 0.392 };
		opened_color[6] = { 0.216, 0.278, 0.310 };
		opened_color[7] = { 0.149, 0.196, 0.219 };

		this.draw_color (ctx, opened_color[0]);
		this.draw_streak (ctx, this.bitopened);

		for
			( int i = int.max (0, this.drawn_segments - 8*opened_color.length)
			; i < this.drawn_segments
			; ++i
			)
		{
			var cidx = (8*opened_color.length - (this.drawn_segments - i)) / 8;
			var segment = segments[i];

			this.draw_color (ctx, opened_color[cidx]);
			ctx.rectangle (segment.opened.x, segment.opened.y, 1, 1);
			ctx.fill ();

			foreach (Gdk.Point closed in segment.closed)
			{
				this.draw_color (ctx, closed_color[cidx]);
				ctx.rectangle (closed.x, closed.y, 1, 1);
				ctx.fill ();
			}
		}

		ctx.restore ();
	}

	private void draw_color (Cairo.Context ctx, Color color)
	{
		ctx.set_source_rgb (color.r, color.g, color.b);
	}

	private void draw_streak (Cairo.Context ctx, int[,] bit)
	{
		for (int y = 0; y < this.model_map.height; ++y)
		{
			int streak = 0;

			for (int x = 0; x < this.model_map.width; ++x)
			{
				if (bit[y, x] > 0 && bit[y, x] <= this.drawn_segments)
				{
					++streak;
				}
				else if (streak > 0)
				{
					ctx.rectangle (x - streak, y, streak, 1);
					streak = 0;
				}
			}

			if (streak > 0)
			{
				ctx.rectangle (this.model_map.width - streak, y, streak, 1);
				streak = 0;
			}
		}

		ctx.fill ();
	}
}

}
