/* $Id$ */
/*
   copyright (C) 2008 by mark de wever <koraq@xs4all.nl>
   part of the battle for wesnoth project http://www.wesnoth.org/

   this program is free software; you can redistribute it and/or modify
   it under the terms of the gnu general public license version 2
   or at your option any later version.
   this program is distributed in the hope that it will be useful,
   but without any warranty.

   see the copying file for more details.
*/

#ifndef GUI_WIDGETS_SCROLLBAR_HPP_INCLUDED
#define GUI_WIDGETS_SCROLLBAR_HPP_INCLUDED

#include "gui/widgets/control.hpp"

namespace gui2 {

/**
 * Base class for a scroll bar.
 *
 * class will be subclassed for the horizontal and vertical scroll bar.
 * It might be subclassed for a slider class.
 *
 * To make this class generic we talk a lot about offset and length and use
 * pure virtual functions. The classes implementing us can use the heights or
 * widths, whichever is applicable.
 */
class tscrollbar_ : public tcontrol
{
public:

	tscrollbar_() :
		tcontrol(COUNT),
		state_(ENABLED),
		item_count_(0),
		item_position_(0),
		visible_items_(1),
		step_size_(1),
		pixels_per_step_(0.0),
		mouse_(0, 0),
		positioner_offset_(0),
		positioner_length_(0),
		callback_positioner_move_(0)
	{
	}

	/**
	 * scroll 'step size'.
	 *
	 * When scrolling we always scroll a 'fixed' amount, these are the
	 * parameters for these amounts.
	 */
	enum tscroll {
		BEGIN,               /**< Go to begin position. */
		ITEM_BACKWARDS,      /**< Go one item towards the begin. */
		HALF_JUMP_BACKWARDS, /**< Go half the visible items towards the begin. */
		JUMP_BACKWARDS,      /**< Go the visibile items towards the begin. */
		END,                 /**< Go to the end position. */
		ITEM_FORWARD,        /**< Go one item towards the end. */
		HALF_JUMP_FORWARD,   /**< Go half the visible items towards the end. */
		JUMP_FORWARD };      /**< Go the visible items towards the end. */

	/**
	 * Sets the item position.
	 *
	 * We scroll a predefined step.
	 *
	 * @param scroll              'step size' to scroll.
	 */
	void scroll(const tscroll scroll);

	/** Is the positioner at the beginning of the scrollbar? */
	bool at_begin() const { return item_position_ == 0; }

	/**
	 * Is the positioner at the and of the scrollbar?
	 *
	 * Note both begin and end might be true at the same time.
	 */
	bool at_end() const
		{ return item_position_ + visible_items_ == item_count_; }

	/***** ***** ***** ***** layout functions ***** ***** ***** *****/

	/** Inherited from tcontrol. */
	void set_size(const tpoint& origin, const tpoint& size);

	/***** ***** ***** ***** Inherited ***** ***** ***** *****/

	/**
	 * Inherited from tevent_executor.
	 *
	 * We only need to track the mouse if it's on the positioner so the normal
	 * enter doesn't help so transfer the control.
	 */
	void mouse_enter(tevent_handler& event) { mouse_move(event); }

	/** Inherited from tevent_executor. */
	void mouse_move(tevent_handler& event);

	/**
	 * Inherited from tevent_executor.
	 *
	 * Leave doesn't have the problem which mouse_enter has so it does it's own
	 * job.
	 */
	void mouse_leave(tevent_handler&);

	/** Inherited from tevent_executor. */
	void mouse_left_button_down(tevent_handler& event);

	/** Inherited from tevent_executor. */
	void mouse_left_button_up(tevent_handler& event);

	/** Inherited from tcontrol. */
	void set_active(const bool active)
		{ if(get_active() != active) set_state(active ? ENABLED : DISABLED); };

	/** Inherited from tcontrol. */
	bool get_active() const { return state_ != DISABLED; }

	/** Inherited from tcontrol. */
	unsigned get_state() const { return state_; }

	/** Inherited from tcontrol. */
	bool does_block_easy_close() const { return true; }

	/***** ***** ***** setters / getters for members ***** ****** *****/

	void set_item_count(const unsigned item_count)
		{ item_count_ = item_count; recalculate(); }
	unsigned get_item_count() const { return item_count_; }

	/**
	 * Note the position isn't guaranteed to be the wanted position
	 * the step size is honoured. The value will be rouded down.
	 */
	void set_item_position(const unsigned item_position);
	unsigned get_item_position() const { return item_position_; }

	unsigned get_visible_items() const { return visible_items_; }
	void set_visible_items(const unsigned visible_items)
		{ visible_items_ = visible_items; recalculate(); }

	unsigned get_step_size() const { return step_size_; }
	void set_step_size(const unsigned step_size)
		{ step_size_ = step_size; recalculate(); }

	void set_callback_positioner_move(void (*callback) (twidget*))
		{ callback_positioner_move_ = callback; }

protected:
	unsigned get_positioner_offset() const { return positioner_offset_; }

	unsigned get_positioner_length() const { return positioner_length_; }

	/** After a recalculation the canvasses also need to be updated. */
	virtual void update_canvas();

private:
	/**
	 * Possible states of the widget.
	 *
	 * Note the order of the states must be the same as defined in settings.hpp.
	 */
	enum tstate { ENABLED, DISABLED, PRESSED, FOCUSSED, COUNT };

	void set_state(const tstate state);
	/**
	 * Current state of the widget.
	 *
	 * The state of the widget determines what to render and how the widget
	 * reacts to certain 'events'.
	 */
	tstate state_;

	/** The number of items the scrollbar 'holds'. */
	unsigned item_count_;

	/** The item the positioner is at, starts at 0. */
	unsigned item_position_;

	/**
	 * The number of items which can be shown at the same time.
	 *
	 * As long as all items are visible we don't need to scroll.
	 */
	unsigned visible_items_;

	/**
	 * Number of items moved when scrolling.
	 *
	 * The step size is the minimum number of items we scroll through when we
	 * move. Normally this value is 1, we can move per item. But for example
	 * sliders want for example to move per 5 items.
	 */
	unsigned step_size_;

	/**
	 * Number of pixels per step.
	 *
	 * The number of pixels the positioner needs to move to go to the next step.
	 * Note if there is too little space it can happen 1 pixel does more than 1
	 * step.
	 */
	float pixels_per_step_;

	/**
	 * The position the mouse was at the last movement.
	 *
	 * This is used during dragging the positioner.
	 */
	tpoint mouse_;

	/**
	 * The start offset of the positioner.
	 *
	 * This takes the offset before in consideration.
	 */
	unsigned positioner_offset_;

	/** The current length of the positioner. */
	unsigned positioner_length_;

 	/** This callback is used when the positioner is moved by the user. */
	void (*callback_positioner_move_) (twidget*);

	/***** ***** ***** ***** Pure virtual functions ***** ***** ***** *****/

	/** Get the length of the scrollbar. */
	virtual unsigned get_length() const = 0;

	/** The minimum length of the positioner. */
	virtual unsigned minimum_positioner_length() const = 0;

	/** The maximum length of the positioner. */
	virtual unsigned maximum_positioner_length() const = 0;

	/**
	 * The number of pixels we can't use since they're used for borders.
	 *
	 * These are the pixels before the widget (left side if horizontal,
	 * top side if vertical).
	 */
	virtual unsigned offset_before() const = 0;

	/**
	 * The number of pixels we can't use since they're used for borders.
	 *
	 * These are the pixels after the widget (right side if horizontal,
	 * bottom side if vertical).
	 */
	virtual unsigned offset_after() const = 0;

	/**
	 * Is the coordinate on the positioner?
	 *
	 * @param coordinate          Coordinate to test whether it's on the
	 *                            positioner.
	 *
	 * @returns                   Whether the location on the positioner is.
	 */
	virtual bool on_positioner(const tpoint& coordinate) const = 0;

	/**
	 * Is the coordinate on the bar?
	 *
	 * @param coordinate          Coordinate to test whether it's on the
	 *                            bar.
	 *
	 * @returns                   Whether the location on the bar is.
	 * @retval -1                 Coordinate is on the bar before positioner.
	 * @retval 0                  Coordinate is not on the bar.
	 * @retval 1                  Coordinate is on the bar after the positioner.
	 */
	virtual int on_bar(const tpoint& coordinate) const = 0;

	/**
	 * Gets the relevent difference in between the two positions.
	 *
	 * This function is used to determine how much the positioner needs to  be
	 * moved.
	 */
	virtual int get_length_difference(
		const tpoint& original, const tpoint& current) const = 0;

	/***** ***** ***** ***** Private functions ***** ***** ***** *****/

	/**
	 * Updates the scrollbar.
	 *
	 * Needs to be called when someting changes eg number of items
	 * or available size. It can only be called once we have a size
	 * otherwise we can't calulate a thing.
	 */
	void recalculate();

	/**
	 * Updates the positioner.
	 *
	 * This is a helper for recalculate().
	 */
	void recalculate_positioner();

	/**
	 * Moves the positioner.
	 *
	 * @param distance           The distance moved, negative to begin, positive
	 *                           to end.
	*/
	void move_positioner(const int distance);

	/** Inherited from tcontrol. */
	void load_config_extra();
};

} // namespace gui2

#endif

