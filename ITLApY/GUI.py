import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import threading
import time
import random
from datetime import datetime

class DarkTheme:
    BG_DARK = "#1a1a1a"
    BG_MEDIUM = "#2b2b2b"
    BG_LIGHT = "#3d3d3d"
    BG_HOVER = "#4a4a4a"
    FG_PRIMARY = "#ffffff"
    FG_SECONDARY = "#dddddd"
    FG_MUTED = "#999999"
    ACCENT_BLUE = "#007acc"
    ACCENT_GREEN = "#00c851"
    ACCENT_RED = "#ff4444"
    ACCENT_ORANGE = "#ff9800"
    BORDER = "#555555"

class ModernButton(tk.Button):
    def __init__(self, parent, **kwargs):
        bg = kwargs.pop('bg_color', DarkTheme.BG_LIGHT)
        hover = kwargs.pop('hover_color', DarkTheme.BG_HOVER)
        fg = kwargs.pop('text_color', DarkTheme.FG_PRIMARY)
        defaults = {
            'bg': bg, 'fg': fg,
            'activebackground': hover,
            'activeforeground': fg,
            'relief': 'flat', 'bd': 0,
            'font': ('Segoe UI', 10),
            'cursor': 'hand2',
            'padx': 20, 'pady': 8
        }
        defaults.update(kwargs)
        super().__init__(parent, **defaults)
        self.bind('<Enter>', lambda e: self.config(bg=hover))
        self.bind('<Leave>', lambda e: self.config(bg=bg))

class ITLAControlGUIFixed:
    def __init__(self, root):
        self.root = root
        self.root.title("ITLA Laser Control System")
        self.root.geometry("900x700")
        self.root.configure(bg=DarkTheme.BG_DARK)
        self.is_demo_running = False
        self.temperature_var = tk.DoubleVar(value=25.30)
        self.frequency_var = tk.DoubleVar(value=193.50)
        self.power_var = tk.DoubleVar(value=5.25)
        self.setup_styles()
        self.setup_gui()

    def setup_styles(self):
        s = ttk.Style()
        # Use 'default' theme to allow full custom color
        s.theme_use('default')
        # Tab background always BG_DARK, text always FG_PRIMARY
        s.configure("Dark.TNotebook",
                    background=DarkTheme.BG_DARK,
                    borderwidth=0)
        s.configure("Dark.TNotebook.Tab",
                    background=DarkTheme.BG_DARK,
                    foreground=DarkTheme.FG_PRIMARY,
                    padding=[20,12],
                    font=('Segoe UI', 10, 'bold'))
        # No change on select or hover: keep black+white
        s.map("Dark.TNotebook.Tab",
              background=[("selected", DarkTheme.BG_DARK),
                          ("active", DarkTheme.BG_DARK)],
              foreground=[("selected", DarkTheme.FG_PRIMARY),
                          ("active", DarkTheme.FG_PRIMARY)])
        # Frames and entries
        s.configure("Dark.TFrame", background=DarkTheme.BG_DARK)
        s.configure("Dark.TEntry",
                    fieldbackground=DarkTheme.BG_LIGHT,
                    foreground=DarkTheme.FG_PRIMARY,
                    borderwidth=1, relief='solid',
                    insertcolor=DarkTheme.FG_PRIMARY)

    def create_header(self, parent, title, subtitle=None):
        h = tk.Frame(parent, bg=DarkTheme.BG_DARK, pady=10)
        h.pack(fill=tk.X, padx=20)
        tk.Label(h, text=title,
                 bg=DarkTheme.BG_DARK, fg=DarkTheme.FG_PRIMARY,
                 font=('Segoe UI',20,'bold')).pack()
        if subtitle:
            tk.Label(h, text=subtitle,
                     bg=DarkTheme.BG_DARK, fg=DarkTheme.FG_MUTED,
                     font=('Segoe UI',11)).pack(pady=(2,0))
        tk.Frame(h, height=2, bg=DarkTheme.ACCENT_BLUE).pack(fill=tk.X,pady=(5,0))

    def setup_gui(self):
        self.create_header(self.root,
                           "ITLA Laser Control System",
                           "Professional Laser Management Interface")
        main = tk.Frame(self.root, bg=DarkTheme.BG_DARK)
        main.pack(fill=tk.BOTH, expand=True, padx=10)
        self.nb = ttk.Notebook(main, style="Dark.TNotebook")
        self.nb.pack(fill=tk.BOTH, expand=True)
        self.setup_connection_tab()
        self.setup_control_tab()
        self.setup_monitor_tab()
        self.create_status_bar()

    def setup_connection_tab(self):
        tab = ttk.Frame(self.nb, style="Dark.TFrame")
        self.nb.add(tab, text="Connection")
        f = tk.Frame(tab, bg=DarkTheme.BG_MEDIUM, padx=15, pady=15)
        f.pack(fill=tk.X, pady=10, padx=10)
        r = tk.Frame(f, bg=DarkTheme.BG_MEDIUM); r.pack(fill=tk.X,pady=5)
        ModernButton(r, text="▶ Start Demo",
                     bg_color=DarkTheme.ACCENT_GREEN,
                     hover_color='#00b84d',
                     command=self.start_demo).pack(side=tk.LEFT)
        ModernButton(r, text="⏹ Stop Demo",
                     bg_color=DarkTheme.ACCENT_RED,
                     hover_color='#e33e3e',
                     command=self.stop_demo).pack(side=tk.LEFT, padx=10)
        inf = tk.Frame(tab, bg=DarkTheme.BG_MEDIUM, padx=15,pady=15)
        inf.pack(fill=tk.X,pady=10,padx=10)
        for lbl,val in [("Port:","COM3 (Demo)"),("Baud:","115200"),("Status:","Ready")]:
            row=tk.Frame(inf,bg=DarkTheme.BG_MEDIUM); row.pack(fill=tk.X,pady=3)
            tk.Label(row,text=lbl,
                     bg=DarkTheme.BG_MEDIUM,fg=DarkTheme.FG_SECONDARY,
                     font=('Segoe UI',10)).pack(side=tk.LEFT)
            tk.Label(row,text=val,
                     bg=DarkTheme.BG_MEDIUM,fg=DarkTheme.FG_PRIMARY,
                     font=('Segoe UI',10,'bold')).pack(side=tk.LEFT,padx=5)

    def setup_control_tab(self):
        tab=ttk.Frame(self.nb,style="Dark.TFrame")
        self.nb.add(tab,text="Laser Control")
        f=tk.Frame(tab,bg=DarkTheme.BG_MEDIUM,padx=15,pady=15)
        f.pack(fill=tk.X,pady=10,padx=10)
        for name,var,cmd in [("Frequency (THz):",self.frequency_var,self.set_frequency),
                            ("Power (dBm):",self.power_var,self.set_power)]:
            r=tk.Frame(f,bg=DarkTheme.BG_MEDIUM); r.pack(fill=tk.X,pady=5)
            tk.Label(r,text=name,
                     bg=DarkTheme.BG_MEDIUM,fg=DarkTheme.FG_SECONDARY,
                     font=('Segoe UI',10)).pack(side=tk.LEFT)
            ttk.Entry(r,textvariable=var,
                      style="Dark.TEntry",
                      font=('Segoe UI',10),width=20).pack(side=tk.LEFT,padx=5)
            ModernButton(r,text="Set",
                         bg_color=DarkTheme.ACCENT_BLUE,
                         command=cmd,width=8).pack(side=tk.LEFT,padx=5)
        r2=tk.Frame(tab,bg=DarkTheme.BG_MEDIUM,padx=15,pady=15);r2.pack(fill=tk.X,pady=10,padx=10)
        ModernButton(r2,text="🔥 LASER ON",
                     bg_color=DarkTheme.ACCENT_GREEN,
                     hover_color='#00b84d',
                     font=('Segoe UI',12,'bold'),
                     width=15,command=self.laser_on).pack(side=tk.LEFT)
        ModernButton(r2,text="⛔ LASER OFF",
                     bg_color=DarkTheme.ACCENT_RED,
                     hover_color='#e33e3e',
                     font=('Segoe UI',12,'bold'),
                     width=15,command=self.laser_off).pack(side=tk.LEFT,padx=20)

    def setup_monitor_tab(self):
        tab=ttk.Frame(self.nb,style="Dark.TFrame")
        self.nb.add(tab,text="Monitor")
        f=tk.Frame(tab,bg=DarkTheme.BG_MEDIUM,padx=15,pady=15)
        f.pack(fill=tk.BOTH,expand=True,pady=10,padx=10)
        for i,(name,var,color) in enumerate([
            ("Frequency",self.frequency_var,DarkTheme.ACCENT_BLUE),
            ("Power",self.power_var,DarkTheme.ACCENT_GREEN),
            ("Temperature",self.temperature_var,DarkTheme.ACCENT_ORANGE),
        ]):
            c=tk.Frame(f,bg=DarkTheme.BG_DARK,pady=10,padx=15)
            c.grid(row=0,column=i,padx=5)
            tk.Label(c,text=name,
                     bg=DarkTheme.BG_DARK,fg=DarkTheme.FG_SECONDARY,
                     font=('Segoe UI',10)).pack()
            tk.Label(c,text=f"{var.get():.3f}",
                     bg=DarkTheme.BG_DARK,fg=color,
                     font=('Segoe UI',14,'bold')).pack(pady=5)
        c2=tk.Frame(f,bg=DarkTheme.BG_DARK,pady=10,padx=15)
        c2.grid(row=0,column=3,padx=5)
        tk.Label(c2,text="Laser Status",
                 bg=DarkTheme.BG_DARK,fg=DarkTheme.FG_SECONDARY,
                 font=('Segoe UI',10)).pack()
        self.laser_status_lbl=tk.Label(c2,text="OFF",
                                       bg=DarkTheme.BG_DARK,fg=DarkTheme.ACCENT_RED,
                                       font=('Segoe UI',14,'bold'))
        self.laser_status_lbl.pack(pady=5)
        log=scrolledtext.ScrolledText(f,bg=DarkTheme.BG_DARK,
                                      fg=DarkTheme.FG_PRIMARY,
                                      insertbackground=DarkTheme.FG_PRIMARY,
                                      font=('Consolas',10),
                                      wrap=tk.WORD,height=10)
        log.grid(row=1,column=0,columnspan=4,pady=(10,0),sticky="nsew")
        self.log=log
        f.rowconfigure(1,weight=1)

    def create_status_bar(self):
        s=tk.Frame(self.root,bg=DarkTheme.BG_MEDIUM,height=30)
        s.pack(side=tk.BOTTOM,fill=tk.X)
        tk.Label(s,text="🟡 Demo Mode Active",
                 bg=DarkTheme.BG_MEDIUM,fg=DarkTheme.FG_PRIMARY,
                 font=('Segoe UI',10),anchor='w').pack(side=tk.LEFT,padx=10)

    def start_demo(self):
        if not self.is_demo_running:
            self.is_demo_running=True
            threading.Thread(target=self.demo_loop,daemon=True).start()
            self.log.insert(tk.END,"[INFO] Demo started\n")

    def stop_demo(self):
        self.is_demo_running=False
        self.log.insert(tk.END,"[INFO] Demo stopped\n")

    def demo_loop(self):
        while self.is_demo_running:
            time.sleep(1)

    def set_frequency(self):
        self.log.insert(tk.END,f"[CMD] SET_FREQUENCY {self.frequency_var.get():.6f}\n")

    def set_power(self):
        self.log.insert(tk.END,f"[CMD] SET_POWER {self.power_var.get():.3f}\n")

    def laser_on(self):
        if messagebox.askyesno("Laser ON","Activate laser?"):
            self.laser_status_lbl.config(text="ON",fg=DarkTheme.ACCENT_GREEN)
            self.log.insert(tk.END,"[CMD] LASER_ON\n")

    def laser_off(self):
        self.laser_status_lbl.config(text="OFF",fg=DarkTheme.ACCENT_RED)
        self.log.insert(tk.END,"[CMD] LASER_OFF\n")

if __name__=="__main__":
    root=tk.Tk()
    app=ITLAControlGUIFixed(root)
    root.mainloop()
