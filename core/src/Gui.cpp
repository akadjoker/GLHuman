#include "Gui.hpp"
#include <algorithm>


GUI *GUI::instance = nullptr;

Skin::Skin()
{


    m_colors[BUTTON_TEXT] = Color(26, 26, 26);         
    m_colors[BUTTON_FACE] = Color(204, 204, 204); 
    m_colors[BUTTON_DOWN] = Color(153, 153, 153);     
    m_colors[SCROLLBAR] = Color(77, 77, 77);           
    m_colors[SCROLLBAR_NOB] = Color(51, 51, 179);     
    m_colors[SCROLLBAR_FILL] = Color(179, 179, 179); 
    m_colors[WINDOW] = Color(128, 128, 128, 128);     
    m_colors[WINDOW_TOP_BAR] = Color(51, 51,255);     
    m_colors[WINDOW_TITLE] = Color(255,255,255);     



    // m_font = nullptr;

    m_font = new Font();
   // m_font->LoadDefaultFont();
    m_font->Load( "assets/font/font1.fnt");
}

Skin::~Skin()
{
    if (m_font)
    {
        m_font->Release();
        delete m_font;
        m_font = nullptr;
    }
}

GUI::GUI()
{
    m_skin=nullptr;
}

GUI::~GUI()
{
   
}
void GUI::init()
{
    if (m_skin)
    {
        delete m_skin;
        m_skin=nullptr;
    }
    m_skin = new Skin();
    anyFocus = false;
}
GUI *GUI::Instance()
{
    if (instance == nullptr)
    {
        instance = new GUI();
        instance->init();
    }
    return instance;
}

void GUI::DestroyInstance()
{
    if (instance)
    {
        instance->Clear();
        delete instance;
        instance = nullptr;
    }
}

void GUI::Clear()
{
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
        delete m_widgets[i];
    }
    m_widgets.clear();
    if (m_skin)
    {
        
        delete m_skin;
        m_skin = nullptr;
    }
}

void GUI::SetSkin(Skin *skin)
{
    if  (m_skin != nullptr)
    {
        delete m_skin;
        m_skin = skin;
    }
}

Window *GUI::CreateWindow(const std::string &title, float x, float y, float width, float height)
{
    Window *window = new Window(title, x, y, width, height);
    window->m_gui=this;
    m_widgets.push_back(window);
    return window;
}

WindowKeyframeEditor *GUI::CreatAnimator(const std::string& title, float x, float y, float width, float height)
{
    WindowKeyframeEditor *window =  new WindowKeyframeEditor(title, x, y, width, height);
    window->m_gui=this;
    m_widgets.push_back(window);
    return window;
}

void GUI::Update(float delta)
{
    
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
        m_widgets[i]->Update(delta);
    
    }
}

void GUI::Render(RenderBatch *batch)
{
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
        m_widgets[i]->Render(batch);
    }
    batch->SetColor(255,255,255,255);
    m_skin->GetFont()->SetSize(14);
}

void GUI::OnMouseMove(int x, int y)
{

    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
            m_widgets[i]->MouseMove(x, y);
    }
}

void GUI::OnMouseDown(int x, int y, int button)
{
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
            m_widgets[i]->MouseDown(x, y, button);
            if (m_widgets[i]->IsFocus())
                anyFocus = true;
    }
}

void GUI::OnMouseUp(int x, int y, int button)
{
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
    
            m_widgets[i]->MouseUp(x, y, button);
            m_widgets[i]->OnReset();

    }
    if (anyFocus)
    {
        anyFocus = false;
    }

}

void GUI::OnKeyDown(Uint32 key)
{
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
        m_widgets[i]->KeyDown(key);
    }
    
}

void GUI::OnKeyUp(Uint32 key)
{
    
    for (unsigned int i = 0; i < m_widgets.size(); i++)
    {
        m_widgets[i]->KeyUp(key);
    }
}



Widget::Widget()
{
    m_parent = NULL;
    m_visible = true;
    m_gui = nullptr;
    m_focus = false;
    iskeyMappped = false;
    m_key = 0;

}

Widget::~Widget()
{
    m_gui= nullptr;
    RemoveAll();
}

void Widget::Render(RenderBatch *batch)
{
  

    OnDraw(batch);
    if (m_visible)
    {
        for (unsigned int i = 0; i < m_children.size(); i++)
        {
            m_children[i]->Render(batch);
        }
    }
}

void Widget::Add(Widget *widget)
{
    m_children.push_back(widget);
    widget->SetParent(this);
}

void Widget::Remove(Widget *widget)
{
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        if (m_children[i] == widget)
        {
            m_children.erase(m_children.begin() + i);
            return;
        }
    }
}

void Widget::RemoveAll()
{
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        delete m_children[i];
    }

    m_children.clear();
}

bool Widget::IsInside(int x, int y) 
{
    return m_bounds.Contains(x, y);
}

float Widget::GetRealX()
{
    if (m_parent == NULL)
        return m_position.x;
    else
        return m_parent->GetRealX() + m_position.x;
}

float Widget::GetRealY()
{
    if (m_parent == NULL)
        return m_position.y;
    else
        return m_parent->GetRealY() + m_position.y;
}

void Widget::Update(float delta)
{
    OnUpdate(delta);
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        m_children[i]->Update(delta);
    }
}
void Widget::SetkeyMap(bool use, Uint32 key)
{
    iskeyMappped = use;
    m_key = key;
}



void Widget::OnUpdate(float delta)
{
    (void)delta;
}

void Widget::MouseMove(int x, int y)
{
   OnMouseMove(x, y);
   for (unsigned int i = 0; i < m_children.size(); i++)
   {
        m_children[i]->MouseMove(x, y);
   }
}

void Widget::MouseDown(int x, int y, int button)
{
    OnMouseDown(x, y, button);
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        m_children[i]->MouseDown(x, y, button);
    }
}

void Widget::MouseUp(int x, int y, int button)
{
    OnMouseUp(x, y, button);
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        m_children[i]->MouseUp(x, y, button);
    }
}

void Widget::KeyDown(Uint32 key)
{
    OnKeyDown(key);
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        m_children[i]->KeyDown(key);
    }
}

void Widget::KeyUp(Uint32 key)
{
    OnKeyUp(key);
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        m_children[i]->KeyUp(key);
    }
}

void Widget::OnMouseMove(int x, int y)
{
    (void)x;
    (void)y;
    
  
}

void Widget::OnMouseDown(int x, int y, int button)
{
    (void)x;
    (void)y;
    (void)button;

}

void Widget::OnMouseUp(int x, int y, int button)
{
    (void)x;
    (void)y;
    (void)button;
    
}

void Widget::OnReset()
{
    for (unsigned int i = 0; i < m_children.size(); i++)
    {
        m_children[i]->OnReset();
    }
}

void Widget::OnKeyDown(Uint32 key)
{
    (void)key;
}

void Widget::OnKeyUp(Uint32 key)
{
    (void)key;
}

Window::Window(const std::string &title, float x, float y, float width, float height): Widget()
{
    m_title = title;
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    m_dragging = false;
    m_dragOffset= Vec2(0.0f, 0.0f);
}

void Window::OnDraw(RenderBatch *batch)
{
    Skin * skin = m_gui->GetSkin();
    Font * font = skin->GetFont();
    font->SetBatch(batch);
    
    

    batch->DrawRectangle((int)GetRealX(),(int) GetRealY()-20, (int)m_size.x,(int) 20, skin->GetColor(WINDOW_TOP_BAR),true);
    if (m_visible)
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, skin->GetColor(WINDOW),true);

    if (m_visible)
        batch->DrawCircle((int)GetRealX()+ (m_size.x-10),(int) GetRealY()-10, 4,  Color(0, 255, 0, 200),true);
    else 
        batch->DrawCircle((int)GetRealX()+ (m_size.x-10),(int) GetRealY()-10, 4,  Color(255, 0, 0, 200),true);

    // batch->DrawRectangle(m_bounds_bar, Color(1,0,0),false);
    // batch->DrawRectangle(m_bounds_hide, Color(1,0,0),false);

    font->SetSize(14);
    font->SetColor(skin->GetColor(WINDOW_TITLE));
    font->Print(m_title.c_str(), GetRealX()+10, GetRealY()-17);
    
}

void Window::OnUpdate(float delta)
{
    (void)delta;
    m_bounds = Rectangle(m_position.x,m_position.y-20, m_size.x, m_size.y);
    m_bounds_bar = Rectangle(GetRealX(), GetRealY()-20, m_size.x, 20);
    m_bounds_hide = Rectangle(GetRealX()+m_size.x-16, GetRealY()-16, 12, 12);

}

void Window::OnMouseMove(int x, int y)
{
    m_focus = m_bounds.Contains(x, y);
    if (m_dragging)
    {
        m_position.x = x - m_dragOffset.x;
        m_position.y = y - m_dragOffset.y;
    }
    
}

void Window::OnMouseDown(int x, int y, int button)
{
    (void)button;
    if (m_bounds_bar.Contains(x, y))
    {
        m_dragging = true;
        m_dragOffset.x = x - GetRealX();
        m_dragOffset.y = y - GetRealY();
    }
    

}

void Window::OnMouseUp(int x, int y, int button)
{
    if (m_bounds_hide.Contains(x, y) && button == 1 )
    {
        m_visible = !m_visible;
    }
    m_dragging = false;
} 

Slider *Window::CreateSlider(bool vertical, float x, float y, float width, float height, float min, float max, float value)
{
    Slider *slider = new Slider(vertical, x, y, width, height, min, max, value);
    slider->m_gui  = this->m_gui;
    Add(slider);
    return slider;
}

Label *Window::CreateLabel(const std::string &text, float x, float y)
{
    
    Label *label = new Label(text, x, y);
    label->m_gui  = this->m_gui;
    Add(label);
    return label;

}

Button *Window::CreateButton(const std::string &text, float x, float y, float width, float height)
{

    Button *button = new Button(text, x, y, width, height);
    button->m_gui  = this->m_gui;
    Add(button);
    return button;

}

CheckBox *Window::CreateCheckBox(const std::string &text, bool state, float x, float y, float width, float height)
{
    
        CheckBox *checkbox = new CheckBox(text, state, x, y, width, height);
        checkbox->m_gui  = this->m_gui;
        Add(checkbox);
        return checkbox;
 
}

KeyframeEditor* Window::CreateKeyframeEditor(float x, float y, float width, float height) {
    KeyframeEditor* editor = new KeyframeEditor(x, y, width, height);
    editor->m_gui = this->m_gui;
    Add(editor);
    return editor;
}

Slider::Slider(bool vertical, float x, float y, float width, float height, float min, float max, float value): Widget()
{
    m_vertical = vertical;
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    m_min = min;
    m_max = max;
    m_value = value;
    m_dragging = false;
    m_grow = 0;

}

void Slider::SetValue(float value)
{
    m_value = value;
    if (OnValueChanged)
        OnValueChanged(m_value);
}

void Slider::OnDraw(RenderBatch *batch)
{
    Skin * skin = m_gui->GetSkin();
    Font * font = skin->GetFont();
  
    batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, skin->GetColor(SCROLLBAR),true);
    if (m_vertical)
    {
        
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_grow, skin->GetColor(SCROLLBAR_FILL),true);
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY()+(int)m_grow, (int)m_size.x, 5, skin->GetColor(SCROLLBAR_NOB),true);
      //  font->DrawText(batch,(m_bounds.x + m_bounds.width) + 5,m_bounds.y+(m_bounds.height/2), Color(1, 1, 1),"%03.f",m_value);
    }
    else
    {
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_grow,(int) m_size.y, skin->GetColor(SCROLLBAR_FILL),true);
        batch->DrawRectangle((int)GetRealX()+(int)m_grow, (int) GetRealY(), 5,(int) m_size.y, skin->GetColor(SCROLLBAR_NOB),true);
        font->DrawText(batch,m_bounds.x + m_bounds.width,m_bounds.y, Color(1, 0, 0),"%03.f",m_value);

    }
    if (m_focus)
    {
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, Color(0.2f, 0.2f, 0.2f),false);
    }


}

void Slider::OnUpdate(float delta)
{
    (void)delta;
    m_bounds = Rectangle(GetRealX(),GetRealY(), m_size.x+1, m_size.y+1);

     if (m_vertical)
    {
        m_grow = (m_value - m_min) / (m_max - m_min) * m_size.y;
       
    }
    else
    {
        m_grow = (m_value - m_min) / (m_max - m_min) * m_size.x;
        
    }

}

void Slider::OnMouseMove(int x, int y)
{

    m_focus = m_bounds.Contains(x, y);

     if (m_dragging)
     {
            if (m_vertical)
            {
                if (m_bounds.Contains(x, y))
                {
                    float pos = y -  GetRealY(); 
                    m_value   =m_min + (m_max - m_min) * (pos) / m_size.y;
                }
            }
            else
            {
                
                if (m_bounds.Contains(x, y))
                {
                    float pos = x -  GetRealX(); 
                    m_value   =m_min + (m_max - m_min) * (pos) / m_size.x;
                }
            }
            if (OnValueChanged)
                OnValueChanged(m_value);
     }

}

void Slider::OnMouseDown(int x, int y, int button)
{
    
   
    if (button==1 && m_focus)
    {
            m_dragging = true;
            if (m_vertical)
            {
                if (m_bounds.Contains(x, y))
                {
                    float pos = y -  GetRealY(); 
                    m_value   =m_min + (m_max - m_min) * (pos) / m_size.y;
                }
            }
            else
            { 
                
                if (m_bounds.Contains(x, y))
                {
                    float pos = x -  GetRealX(); 
                    m_value   =m_min + (m_max - m_min) * (pos) / m_size.x;
                }
            }
            if (OnValueChanged)
                OnValueChanged(m_value);
    }


}

void Slider::OnMouseUp(int x, int y, int button)
{
    (void)x;
    (void)y;
    (void)button;

    m_dragging = false;

}


Label::Label(const std::string &text, float x, float y): Widget()
{
    m_text = text;
    m_position.x = x;
    m_position.y = y;
    
}

void Label::OnDraw(RenderBatch *batch)
{
    Skin * skin = m_gui->GetSkin();
    Font * font = skin->GetFont();
    
    font->DrawText(batch, m_text.c_str(), GetRealX() , GetRealY() ,skin->GetColor(LABEL));
}

Button::Button(const std::string &text, float x, float y, float width, float height): Widget()
{
    m_text = text;
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    m_down = false;
    m_focus = false;
    m_hover = false;

    text_width  = 0;
    text_height = 0;
}

void Button::OnDraw(RenderBatch *batch)
{
    Skin * skin = m_gui->GetSkin();
    Font * font = skin->GetFont();
    
    if (m_down)
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, skin->GetColor(BUTTON_DOWN),true);
    else
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, skin->GetColor(BUTTON_FACE),true);
    if (m_hover)
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, Color(0.2f, 0.2f, 0.2f),false);

    int w  = font->GetTextWidth(m_text.c_str());
    float h = font->GetHeight();

    //Log(2, "w %f h %f", w, h);
    font->SetColor(skin->GetColor(BUTTON_TEXT));
    font->DrawText(batch, m_text.c_str(), GetRealX() + (m_size.x / 2) - (w / 2), GetRealY(), skin->GetColor(BUTTON_TEXT));
}

void Button::OnUpdate(float delta)
{
    (void)delta;
    if (text_width == 0 || text_height == 0)
    {
        Skin * skin = m_gui->GetSkin();
        Font * font = skin->GetFont();
        text_width = font->GetTextWidth(m_text.c_str());
        text_height = font->GetHeight();
    }
   

      m_bounds = Rectangle(GetRealX(),GetRealY(), m_size.x+1, m_size.y+1);

      
  
}

void Button::OnMouseMove(int x, int y)
{
    m_focus = m_bounds.Contains(x, y);
    m_hover = m_focus;
}

void Button::OnMouseDown(int x, int y, int button)
{
    if (button == 1)
    if (m_bounds.Contains(x, y))
    {
        m_down = true;

      
    }
}

void Button::OnMouseUp(int x, int y, int button)
{
    if (button == 1)
    if (m_bounds.Contains(x, y))
    {
        m_down = false;
        if (OnClick)
            OnClick();
    }
}

void Button::OnKeyDown(Uint32 key)
{
        if (iskeyMappped && m_gui!=nullptr)  
        if (key == m_key && !m_down)
        {
            m_down = true;
            if (OnDown)
                OnDown();
        }

}

void Button::OnKeyUp(Uint32 key)
{
        if (iskeyMappped && m_gui!=nullptr)  
        if (key == m_key && m_down)
        {
            m_down = false;
            if (OnClick)
                OnClick();
        }
}

CheckBox::CheckBox(const std::string &text, bool state, float x, float y, float width, float height): Widget()
{
    m_text = text;
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    m_checked = state;
    m_down = false;
    m_focus = false;
    m_hover = false;

}

void CheckBox::OnDraw(RenderBatch *batch)
{
    Skin * skin = m_gui->GetSkin();
    Font * font = skin->GetFont();
    
    batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, skin->GetColor(BUTTON_FACE),true);
    if (m_hover)
        batch->DrawRectangle((int)GetRealX(),(int) GetRealY(), (int)m_size.x,(int) m_size.y, Color(0.2f, 0.2f, 0.2f),false);

    float bw = m_size.x / 2;
    float bh = m_size.y / 2;

    if (m_checked)
        batch->DrawRectangle((int)GetRealX()+(bw/2),(int) GetRealY()+(bh/2), bw,bh, Color(0.2f, 0.2f, 0.2f),true);

    int w  = font->GetTextWidth(m_text.c_str());
    float h = font->GetHeight();

    //Log(2, "w %f h %f", w, h);

    font->DrawText(batch, m_text.c_str(), GetRealX() + (m_size.x + 1), GetRealY() + h, Color::WHITE);
}

void CheckBox::OnUpdate(float delta)
{
    (void)delta;
  

      m_bounds = Rectangle(GetRealX(),GetRealY(), m_size.x+1, m_size.y+1);

      
  
}

void CheckBox::OnMouseMove(int x, int y)
{
    m_focus = m_bounds.Contains(x, y);
    m_hover = m_focus;
}

void CheckBox::OnMouseDown(int x, int y, int button)
{
    if (button == 1)
    if (m_bounds.Contains(x, y))
    {
        m_down = true;

      
    }
}

void CheckBox::OnMouseUp(int x, int y, int button)
{
    if (button == 1)
    if (m_bounds.Contains(x, y))
    {
        m_checked = !m_checked;
        m_down = false;
        if (OnClick)
            OnClick();
    }
}

void CheckBox::OnKeyDown(Uint32 key)
{
        if (iskeyMappped && m_gui!=nullptr)  
        if (key == m_key && !m_down)
        {
            m_down = true;
            if (OnDown)
                OnDown();
        }

}

void CheckBox::OnKeyUp(Uint32 key)
{
        if (iskeyMappped && m_gui!=nullptr)  
        if (key == m_key && m_down)
        {
            m_down = false;
            if (OnClick)
                OnClick();
        }
}



KeyframeEditor::KeyframeEditor(float x, float y, float width, float height) : Widget() {
    m_position = Vec2(x, y);
    m_size = Vec2(width, height);
    m_currentFrame = 0;
    m_totalFrames = 100; // 10 segundos a 10 frames por segundo
    m_trackHeight = 30;
    m_draggingFrame = false;
    m_selectedKeyframe = nullptr;
    m_timelineScale = 1.0f;

    // Criar tracks baseados nas propriedades do objeto
    CreateTrack("Head Rotation", -180, 180);
    CreateTrack("Torso Rotation", -180, 180);
    CreateTrack("Upper Arm L", -180, 180);
    CreateTrack("Upper Arm R", -180, 180);
    CreateTrack("Forearm L", -180, 180);
    CreateTrack("Forearm R", -180, 180);
    CreateTrack("Thigh L", -180, 180);
    CreateTrack("Thigh R", -180, 180);
    CreateTrack("Calf L", -180, 180);
    CreateTrack("Calf R", -180, 180);
}

void KeyframeEditor::CreateTrack(const std::string& name, float min, float max) {
    Track track;
    track.name = name;
    track.minValue = min;
    track.maxValue = max;
    track.expanded = true;
    m_tracks.push_back(track);
}

void KeyframeEditor::OnDraw(RenderBatch* batch) {
    Skin* skin = m_gui->GetSkin();
    Font* font = skin->GetFont();
    
    // Desenhar background
    batch->DrawRectangle(GetRealX(), GetRealY(), m_size.x, m_size.y, 
                        skin->GetColor(WINDOW), true);

    // Desenhar timeline ruler
    float rulerHeight = 20;
    batch->DrawRectangle(GetRealX(), GetRealY(), m_size.x, rulerHeight, 
                        skin->GetColor(WINDOW_TOP_BAR), true);

    // Desenhar marcadores de frame a cada 10 frames
    for(int i = 0; i <= m_totalFrames; i += 10) {
        float x = GetRealX() + (i * m_size.x / m_totalFrames);
        batch->DrawRectangle(x, GetRealY(), 1, rulerHeight, Color::WHITE, true);
        font->DrawText(batch, std::to_string(i).c_str(), x + 2, GetRealY() + 2, Color::WHITE);
    }

    // Desenhar tracks
    float y = GetRealY() + rulerHeight;
    for(const auto& track : m_tracks) {
        if(!track.expanded) continue;

        // Track background
        batch->DrawRectangle(GetRealX(), y, m_size.x, m_trackHeight, 
                            skin->GetColor(BUTTON_FACE), true);
        
        // Track name
        font->DrawText(batch, track.name.c_str(), GetRealX() + 5, y + 5, 
                      skin->GetColor(BUTTON_TEXT));

        // Desenhar keyframes
        for(const auto& keyframe : track.keyframes) {
            float x = GetRealX() + (keyframe.frame * m_size.x / m_totalFrames);
            float keyY = y + (m_trackHeight / 2);
            
            // Keyframe diamond
            Color keyColor = keyframe.selected ? Color::YELLOW : Color::WHITE;
            batch->DrawRectangle(x - 4, keyY - 4, 8, 8, keyColor, true);
        }

        y += m_trackHeight;
    }

    // Desenhar cursor de frame atual
    float cursorX = GetRealX() + (m_currentFrame * m_size.x / m_totalFrames);
    batch->DrawRectangle(cursorX, GetRealY(), 2, m_size.y, Color::RED, true);
}

void KeyframeEditor::OnMouseDown(int x, int y, int button) {
    if(button != 1) return;

    float rulerY = GetRealY();
    float tracksY = rulerY + 20;

    // Clique na régua do timeline
    if(y >= rulerY && y < tracksY) {
        m_currentFrame = ((x - GetRealX()) / m_size.x) * m_totalFrames;
        m_currentFrame = Clamp(m_currentFrame, 0, m_totalFrames);
        return;
    }

    // Clique nos tracks
    float trackY = tracksY;
    for(auto& track : m_tracks) {
        if(!track.expanded) continue;

        if(y >= trackY && y < trackY + m_trackHeight) {
            // Adicionar novo keyframe com double click
            if(button == 1) {
                int frame = ((x - GetRealX()) / m_size.x) * m_totalFrames;
                frame = Clamp(frame, 0, m_totalFrames);
                
                Keyframe keyframe;
                keyframe.frame = frame;
                keyframe.value = 0;
                keyframe.selected = true;
                
                track.keyframes.push_back(keyframe);
                m_selectedKeyframe = &track.keyframes.back();
            }
            break;
        }
        trackY += m_trackHeight;
    }
}

void KeyframeEditor::OnMouseMove(int x, int y) {
    if(m_selectedKeyframe && (x - GetRealX()) >= 0 && (x - GetRealX()) <= m_size.x) {
        m_selectedKeyframe->frame = ((x - GetRealX()) / m_size.x) * m_totalFrames;
        m_selectedKeyframe->frame = Clamp(m_selectedKeyframe->frame, 0, m_totalFrames);
    }
}

void KeyframeEditor::OnMouseUp(int x, int y, int button) {
    if(button == 1) {
        m_selectedKeyframe = nullptr;
    }
}



WindowKeyframeEditor::WindowKeyframeEditor(const std::string& title, float x, float y, float width, float height)
    : Window(title, x, y, width, height) {
    m_currentFrame = 0;
    m_totalFrames = 100;
    m_playing = false;
    m_playbackSpeed = 1.0f;
    m_selectedKeyframe = nullptr;
    m_draggingFrame = false;
    m_frameWidth = 10.0f;

    CreateDefaultControls();
}

void WindowKeyframeEditor::CreateDefaultControls() {
    // Criar controles básicos
    m_playButton = CreateButton("Play", 10, 10, 60, 20);
    m_stopButton = CreateButton("Stop", 80, 10, 60, 20);
    m_captureButton = CreateButton("Capture", 150, 10, 60, 20);
    m_clearButton = CreateButton("Clear", 220, 10, 60, 20);
    
    m_frameSlider = CreateSlider(false, 10, 40, m_size.x - 20, 20, 0, m_totalFrames, 0);

    // Configurar callbacks
    m_playButton->OnClick = [this]() {
        m_playing = !m_playing;
        m_playButton->SetText(m_playing ? "Pause" : "Play");
    };

    m_stopButton->OnClick = [this]() {
        m_playing = false;
        m_currentFrame = 0;
        m_frameSlider->SetValue(0);
        m_playButton->SetText("Play");
    };

    m_captureButton->OnClick = [this]() {
        CaptureKeyframe();
    };

    m_clearButton->OnClick = [this]() {
        Clear();
    };

    m_frameSlider->OnValueChanged = [this](float value) {
        m_currentFrame = (int)value;
        if(OnFrameChanged) {
            OnFrameChanged(m_currentFrame);
        }
    };

    m_timelineArea = Rectangle(10, 70, m_size.x - 20, m_size.y - 80);
}

void WindowKeyframeEditor::CaptureKeyframe() {
    AnimationKeyframe keyframe;
    keyframe.frame = m_currentFrame;
    keyframe.selected = false;
    //get windo values

    m_keyframes.push_back(keyframe);
    SortKeyframes();
}

void WindowKeyframeEditor::OnDraw(RenderBatch* batch) {
    Window::OnDraw(batch);
    

    batch->DrawRectangle(GetRealX() + m_timelineArea.x, GetRealY() + m_timelineArea.y, 
                        m_timelineArea.width, m_timelineArea.height, 
                        Color(50, 50, 50), true);


    for(int i = 0; i <= m_totalFrames; i += 10) {
        float x = GetRealX() + m_timelineArea.x + (i * m_timelineArea.width / m_totalFrames);
        batch->DrawRectangle(x, GetRealY() + m_timelineArea.y, 
                           1, m_timelineArea.height, Color(100, 100, 100), true);
    }


    for(const auto& keyframe : m_keyframes) {
        float x = GetRealX() + m_timelineArea.x + 
                 (keyframe.frame * m_timelineArea.width / m_totalFrames);
        
        Color keyframeColor = keyframe.selected ? Color::YELLOW : Color::WHITE;
        batch->DrawRectangle(x - 4, GetRealY() + m_timelineArea.y + m_timelineArea.height/2 - 4,
                           8, 8, keyframeColor, true);
    }


    float cursorX = GetRealX() + m_timelineArea.x + 
                   (m_currentFrame * m_timelineArea.width / m_totalFrames);
    batch->DrawRectangle(cursorX, GetRealY() + m_timelineArea.y,
                       2, m_timelineArea.height, Color::RED, true);
}

void WindowKeyframeEditor::OnUpdate(float delta) {
    Window::OnUpdate(delta);
    
    if(m_playing) {
        m_currentFrame++;
        if(m_currentFrame >= m_totalFrames) {
            m_currentFrame = 0;
        }
        m_frameSlider->SetValue(m_currentFrame);
    }
}

void WindowKeyframeEditor::Clear() {
    m_keyframes.clear();
    m_currentFrame = 0;
    m_frameSlider->SetValue(0);
    m_playing = false;
    m_playButton->SetText("Play");
}

void WindowKeyframeEditor::SortKeyframes() {
    std::sort(m_keyframes.begin(), m_keyframes.end(), 
             [](const AnimationKeyframe& a, const AnimationKeyframe& b) {
                 return a.frame < b.frame;
             });
}