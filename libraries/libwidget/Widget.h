#pragma once

#include <libsystem/algebra/Rect.h>
#include <libsystem/utils/List.h>

#include <libgraphic/Font.h>

#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>

struct Widget;
struct Painter;
struct Window;

enum LayoutType
{
    LAYOUT_STACK,
    LAYOUT_GRID,
    LAYOUT_VGRID,
    LAYOUT_HGRID,
    LAYOUT_VFLOW,
    LAYOUT_HFLOW,
};

struct Layout
{
    LayoutType type;

    int hcell;
    int vcell;
    Vec2i spacing;
};

#define STACK() \
    ((Layout){LAYOUT_STACK, 0, 0, Vec2i::zero()})

#define GRID(_hcell, _vcell, _hspacing, _vspacing) \
    ((Layout){LAYOUT_GRID, (_hcell), (_vcell), Vec2i((_hspacing), (_vspacing))})

#define VGRID(_vspacing) \
    ((Layout){LAYOUT_VGRID, 0, 0, Vec2i(0, (_vspacing))})

#define HGRID(_hspacing) \
    ((Layout){LAYOUT_HGRID, 0, 0, Vec2i((_hspacing), 0)})

#define VFLOW(_vspacing) \
    ((Layout){LAYOUT_VFLOW, 0, 0, Vec2i(0, (_vspacing))})

#define HFLOW(_hspacing) \
    ((Layout){LAYOUT_HFLOW, 0, 0, Vec2i((_hspacing), 0)})

struct WidgetColor
{
    bool overwritten;
    Color color;
};

struct WidgetMetrics
{
    Vec2i origine;

    Recti overflow;
    Recti bound;
    Recti content;
};

class Widget
{
private:
    bool _enabled = true;
    Recti _bound{};
    int _flags = 0;

    int _max_height = 0;
    int _max_width = 0;
    int _min_height = 0;
    int _min_width = 0;

    Insetsi _outsets{};
    Insetsi _insets{};

    WidgetColor _colors[__THEME_COLOR_COUNT] = {};
    Layout _layout = {};
    RefPtr<Font> _font;

    CursorState _cursor = CURSOR_DEFAULT;

    EventHandler _handlers[EventType::__COUNT] = {};

    struct Widget *_parent = {};
    struct Window *_window = {};

    Vector<Widget *> _childs = {};

public:
    static constexpr auto FILL = (1 << 0);
    static constexpr auto GREEDY = (1 << 1);
    static constexpr auto SQUARE = (1 << 2);
    static constexpr auto NO_MOUSE_HIT = (1 << 3);

    void id(String id);

    RefPtr<Font> font()
    {
        if (!_font)
        {
            _font = Font::get("sans").take_value();
        }

        return _font;
    }

    void font(RefPtr<Font> font)
    {
        _font = font;
    }

    Color color(ThemeColorRole role);

    void color(ThemeColorRole role, Color color);

    Recti content_bound() const { return bound().shrinked(_insets); }

    Recti bound() const { return _bound; }

    void bound(Recti value) { _bound = value; }

    Insetsi outsets() const { return _outsets; }

    void outsets(Insetsi outsets)
    {
        _outsets = outsets;
        should_relayout();
    }

    Insetsi insets() const { return _insets; }

    void insets(Insetsi insets)
    {
        _insets = insets;
        should_relayout();
    }

    WidgetMetrics metrics() const
    {
        Vec2i origine{
            _outsets.left() + bound().x(),
            _outsets.top() + bound().y(),
        };

        Recti bound = Widget::bound().shrinked(_outsets).size();
        Recti content = bound.shrinked(_insets);
        Recti overflow = content.expended(_outsets);

        return {
            origine,
            overflow,
            bound,
            content,
        };
    }

    void layout(Layout layout) { _layout = layout; }

    void flags(int attributes) { _flags = attributes; }

    int flags() { return _flags; }

    void window(Window *window)
    {
        assert(!_window);

        _window = window;
    }

    Window *window()
    {
        return _window;
    }

    void min_height(int value) { _min_height = value; }
    int min_height() { return _min_height; }

    void max_height(int value) { _max_height = value; }
    int max_height() { return _max_height; }

    void min_width(int value) { _min_width = value; }
    int min_width() { return _min_width; }

    void max_width(int value) { _max_width = value; }
    int max_width() { return _max_width; }

    CursorState cursor();

    void cursor(CursorState cursor);

    /* --- subclass API ----------------------------------------------------- */

    Widget(Widget *parent);

    virtual ~Widget();

    virtual void paint(Painter &, const WidgetMetrics &, const Recti &){};

    virtual void paint(Painter &painter, Recti rectangle);

    virtual void event(Event *event);

    virtual void do_layout();

    virtual Vec2i size();

    /* --- Enable/Disable state --------------------------------------------- */

    bool enabled();

    bool disabled();

    void enable();

    void disable();

    void disable_if(bool condition);

    void enable_if(bool condition);

    /* --- Childs ----------------------------------------------------------- */

    Widget *child_at(Vec2i position);

    void add_child(Widget *child);

    void remove_child(Widget *child);

    void clear_children();

    /* --- Focus state ------------------------------------------------------ */

    bool focused();

    void focus();

    /* --- Paint ------------------------------------------------------------ */

    void repaint(Painter &painter, Recti rectangle);

    void should_repaint();

    void should_repaint(Recti rectangle);

    /* --- Layout ----------------------------------------------------------- */

    void relayout();

    void should_relayout();

    Vec2i compute_size();

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);

    void dispatch_event(Event *event);
};
