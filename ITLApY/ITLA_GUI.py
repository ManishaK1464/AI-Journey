import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import serial
import serial.tools.list_ports
import threading
import json
from datetime import datetime

class DarkTheme:
    BG_DARK = "#1a1a1a"
    BG_MEDIUM = "#2b2b2b"
    BG_LIGHT = "#3d3d3d"
    BG_HOVER = "#4a4a4a"
    FG_PRIMARY = "#ffffff"
    FG_SECONDARY = "#dddddd"
    ACCENT_BLUE = "#007acc"
    ACCENT_GREEN = "#00c851"
    ACCENT_RED = "#ff4444"

class ModernButton(tk.Button):
    def __init__(self, parent, **kwargs):
        bg = kwargs.pop('bg_color', DarkTheme.BG_LIGHT)
        hv = kwargs.pop('hover_color', DarkTheme.BG_HOVER)
        fg = kwargs.pop('text_color', DarkTheme.FG_PRIMARY)
        opts = {'bg': bg, 'fg': fg, 'activebackground': hv, 'activeforeground': fg,
                'relief':'flat','bd':0,'font':('Segoe UI',10),'cursor':'hand2','padx':20,'pady':8}
        opts.update(kwargs)
        super().__init__(parent, **opts)
        self.bind('<Enter>', lambda e: self.config(bg=hv))
        self.bind('<Leave>', lambda e: self.config(bg=bg))

class ITLAControlGUI:
    def __init__(self, root):
        self.root = root
        root.title("ITLA Laser Control System")
        root.geometry("900x700")
        root.configure(bg=DarkTheme.BG_DARK)

        self.ser = None

        self.freq_var = tk.DoubleVar(value=193.50)
        self.power_var = tk.DoubleVar(value=0.0)

        self.setup_styles()
        self.build_gui()
        self.reader_thread = None
        self.running = False

    def setup_styles(self):
        s = ttk.Style()
        s.theme_use('default')
        s.configure("Dark.TNotebook", background=DarkTheme.BG_DARK, borderwidth=0)
        s.configure("Dark.TNotebook.Tab",
                    background=DarkTheme.BG_DARK, foreground=DarkTheme.FG_PRIMARY,
                    padding=[20,12], font=('Segoe UI',10,'bold'))
        s.map("Dark.TNotebook.Tab",
              background=[("selected",DarkTheme.BG_DARK),("active",DarkTheme.BG_DARK)],
              foreground=[("selected",DarkTheme.FG_PRIMARY),("active",DarkTheme.FG_PRIMARY)])
        s.configure("Dark.TFrame", background=DarkTheme.BG_DARK)
        s.configure("Dark.TEntry",
                    fieldbackground=DarkTheme.BG_LIGHT, foreground=DarkTheme.FG_PRIMARY,
                    borderwidth=1, relief='solid', insertcolor=DarkTheme.FG_PRIMARY)

    def build_gui(self):
        nb = ttk.Notebook(self.root, style="Dark.TNotebook")
        nb.pack(fill=tk.BOTH, expand=True, pady=10)

        # Connection Tab
        tab1 = ttk.Frame(nb, style="Dark.TFrame")
        nb.add(tab1, text="Connection")
        self.build_connection(tab1)

        # Control Tab
        tab2 = ttk.Frame(nb, style="Dark.TFrame")
        nb.add(tab2, text="Laser Control")
        self.build_control(tab2)

        # Monitor Tab
        tab3 = ttk.Frame(nb, style="Dark.TFrame")
        nb.add(tab3, text="Monitor")
        self.build_monitor(tab3)

    def build_connection(self, frame):
        f = tk.Frame(frame, bg=DarkTheme.BG_MEDIUM, padx=15, pady=15)
        f.pack(fill=tk.X, pady=10, padx=10)
        tk.Label(f, text="Port:", bg=DarkTheme.BG_MEDIUM, fg=DarkTheme.FG_SECONDARY,
                 font=('Segoe UI',10)).grid(row=0,column=0,sticky='w')
        self.port_cb = ttk.Combobox(f, state="readonly", width=20)
        self.port_cb.grid(row=0,column=1,padx=5)
        tk.Label(f, text="Baud:", bg=DarkTheme.BG_MEDIUM, fg=DarkTheme.FG_SECONDARY,
                 font=('Segoe UI',10)).grid(row=0,column=2,sticky='w',padx=(20,0))
        self.baud_cb = ttk.Combobox(f, values=["9600","19200","38400","57600","115200"], width=10)
        self.baud_cb.set("115200")
        self.baud_cb.grid(row=0,column=3,padx=5)
        ModernButton(f, text="Refresh", command=self.refresh_ports).grid(row=0,column=4,padx=10)
        ModernButton(f, text="Connect", bg_color=DarkTheme.ACCENT_GREEN, command=self.connect).grid(row=0,column=5,padx=5)
        ModernButton(f, text="Disconnect", bg_color=DarkTheme.ACCENT_RED, command=self.disconnect).grid(row=0,column=6,padx=5)
        self.status_lbl = tk.Label(frame, text="Not connected",
                                   bg=DarkTheme.BG_DARK, fg=DarkTheme.FG_PRIMARY,
                                   font=('Segoe UI',10))
        self.status_lbl.pack(anchor='w',padx=20)

    def build_control(self, frame):
        f = tk.Frame(frame, bg=DarkTheme.BG_MEDIUM, padx=15, pady=15)
        f.pack(fill=tk.X,pady=10,padx=10)
        for i,(label,var,cmd) in enumerate([
            ("Frequency (THz):", self.freq_var, self.send_frequency),
            ("Power (dBm):", self.power_var, self.send_power),
        ]):
            r = tk.Frame(f, bg=DarkTheme.BG_MEDIUM)
            r.pack(fill=tk.X,pady=5)
            tk.Label(r,text=label, bg=DarkTheme.BG_MEDIUM, fg=DarkTheme.FG_SECONDARY,
                     font=('Segoe UI',10)).pack(side=tk.LEFT)
            ttk.Entry(r,textvariable=var, style="Dark.TEntry",
                      font=('Segoe UI',10), width=20).pack(side=tk.LEFT,padx=5)
            ModernButton(r,text="Set", bg_color=DarkTheme.ACCENT_BLUE,
                         command=cmd,width=8).pack(side=tk.LEFT,padx=5)
        r2 = tk.Frame(frame, bg=DarkTheme.BG_MEDIUM, padx=15, pady=15)
        r2.pack(fill=tk.X,pady=10,padx=10)
        ModernButton(r2,text="🔥 LASER ON",
                     bg_color=DarkTheme.ACCENT_GREEN,
                     command=self.send_laser_on,
                     font=('Segoe UI',12,'bold'),
                     width=15).pack(side=tk.LEFT)
        ModernButton(r2,text="⛔ LASER OFF",
                     bg_color=DarkTheme.ACCENT_RED,
                     command=self.send_laser_off,
                     font=('Segoe UI',12,'bold'),
                     width=15).pack(side=tk.LEFT,padx=20)

    def build_monitor(self, frame):
        f = tk.Frame(frame, bg=DarkTheme.BG_MEDIUM, padx=15,pady=15)
        f.pack(fill=tk.BOTH,expand=True,pady=10,padx=10)
        self.values = {}
        for i,(name,color) in enumerate([
            ("Frequency", DarkTheme.ACCENT_BLUE),
            ("Power", DarkTheme.ACCENT_GREEN),
            ("Temperature", DarkTheme.ACCENT_ORANGE),
        ]):
            c = tk.Frame(f,bg=DarkTheme.BG_DARK,pady=10,padx=15)
            c.grid(row=0,column=i,padx=5)
            tk.Label(c,text=name, bg=DarkTheme.BG_DARK, fg=DarkTheme.FG_SECONDARY,
                     font=('Segoe UI',10)).pack()
            lbl = tk.Label(c,text="--", bg=DarkTheme.BG_DARK, fg=color,
                           font=('Segoe UI',14,'bold'))
            lbl.pack(pady=5)
            self.values[name] = lbl
        c2 = tk.Frame(f,bg=DarkTheme.BG_DARK,pady=10,padx=15)
        c2.grid(row=0,column=3,padx=5)
        tk.Label(c2,text="Laser Status", bg=DarkTheme.BG_DARK, fg=DarkTheme.FG_SECONDARY,
                 font=('Segoe UI',10)).pack()
        self.laser_lbl = tk.Label(c2,text="OFF", bg=DarkTheme.BG_DARK, fg=DarkTheme.ACCENT_RED,
                                  font=('Segoe UI',14,'bold'))
        self.laser_lbl.pack(pady=5)
        self.log = scrolledtext.ScrolledText(f, bg=DarkTheme.BG_DARK,
                                             fg=DarkTheme.FG_PRIMARY,
                                             insertbackground=DarkTheme.FG_PRIMARY,
                                             font=('Consolas',10), wrap=tk.WORD, height=10)
        self.log.grid(row=1,column=0,columnspan=4,pady=(10,0),sticky="nsew")
        f.rowconfigure(1, weight=1)

    def create_status_bar(self):
        s = tk.Frame(self.root,bg=DarkTheme.BG_MEDIUM,height=30)
        s.pack(side=tk.BOTTOM,fill=tk.X)
        self.status = tk.Label(s,text="Disconnected",
                               bg=DarkTheme.BG_MEDIUM,fg=DarkTheme.FG_PRIMARY,
                               font=('Segoe UI',10),anchor='w')
        self.status.pack(side=tk.LEFT,padx=10)

    def refresh_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_cb['values'] = ports
        if ports:
            self.port_cb.set(ports[0])

    def connect(self):
        port = self.port_cb.get()
        baud = int(self.baud_cb.get())
        try:
            self.ser = serial.Serial(port,baud,timeout=1)
            self.status.config(text=f"Connected to {port}")
            self.start_reader()
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def disconnect(self):
        self.running = False
        if self.ser:
            self.ser.close()
        self.status.config(text="Disconnected")

    def start_reader(self):
        self.running = True
        self.reader_thread = threading.Thread(target=self.read_loop,daemon=True)
        self.reader_thread.start()

    def read_loop(self):
        while self.running:
            line = self.ser.readline().decode().strip()
            if line.startswith("{") and line.endswith("}"):
                data = json.loads(line)
                self.root.after(0,self.update_values,data)
            else:
                self.root.after(0,self.log.insert,tk.END,f"{line}\n")

    def update_values(self,data):
        self.values['Frequency'].config(text=f"{data['freq']:.6f}")
        self.values['Power'].config(text=f"{data['power']:.3f}")
        self.values['Temperature'].config(text=f"{data['temp']:.2f}")
        # Laser status remains manual

    def send_command(self,cmd):
        if not self.ser:
            messagebox.showwarning("Warning","Not connected")
            return
        self.ser.write(f"{cmd}\n".encode())
        self.log.insert(tk.END,f"[TX] {cmd}\n")

    def send_frequency(self):
        self.send_command(f"SET_FREQUENCY {self.frequency_var.get():.6f}")

    def send_power(self):
        self.send_command(f"SET_POWER {self.power_var.get():.3f}")

    def send_laser_on(self):
        if messagebox.askyesno("Laser ON","Activate laser?"):
            self.send_command("LASER_ON")
            self.laser_lbl.config(text="ON",fg=DarkTheme.ACCENT_GREEN)

    def send_laser_off(self):
        self.send_command("LASER_OFF")
        self.laser_lbl.config(text="OFF",fg=DarkTheme.ACCENT_RED)

if __name__=="__main__":
    root = tk.Tk()
    gui = ITLAControlGUI(root)
    gui.refresh_ports()
    root.mainloop()
