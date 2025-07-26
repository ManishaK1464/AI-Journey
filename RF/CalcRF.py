import tkinter as tk
from tkinter import ttk, messagebox, font
import math

# Constants
c = 3e8  # Speed of light in m/s

def to_superscript(exp_str):
    sup_map = {'0':'⁰','1':'¹','2':'²','3':'³','4':'⁴','5':'⁵','6':'⁶','7':'⁷','8':'⁸','9':'⁹','-':'⁻'}
    return ''.join(sup_map.get(ch, ch) for ch in exp_str)
    
def sci_notation_superscript(num):
    s = f"{num:.2e}"
    parts = s.split('e')
    mantissa = parts[0]
    exponent = parts[1].replace('+','')
    return f"{mantissa}×10{to_superscript(exponent)}"

def format_number(num):
    if abs(num) >= 1e5 or abs(num) < 1e-3:
        return sci_notation_superscript(num)
    else:
        return f"{num:,.4f}"

def calculate():
    try:
        freq_ghz = float(freq_entry.get())
        dist_km = float(dist_entry.get())

        if freq_ghz <= 0 or dist_km <= 0:
            raise ValueError("Frequency and distance must be positive numbers.")

        freq_hz = freq_ghz * 1e9
        dist_m = dist_km * 1000

        wavelength = c / freq_hz

        freq_mhz = freq_ghz * 1e3
        fspl = 20 * math.log10(dist_km) + 20 * math.log10(freq_mhz) + 32.44

        for widget in result_frame.winfo_children():
            widget.destroy()

        step1_title = tk.Label(result_frame, text="Step 1: Calculate Wavelength (λ)", font=bold_font, fg="#1E90FF", bg="black")
        step1_title.pack(anchor='w')
        formula1 = tk.Label(result_frame, text="Formula: λ = c / f", fg="#2E8B57", bg="black")
        formula1.pack(anchor='w')
        constants1 = tk.Label(result_frame, text=f"Constants: c = {sci_notation_superscript(c)} m/s, Frequency f = {sci_notation_superscript(freq_hz)} Hz", bg="black")
        constants1.pack(anchor='w')
        calculation1 = tk.Label(result_frame, text=f"Calculation: λ = {sci_notation_superscript(c)} / {sci_notation_superscript(freq_hz)} = {format_number(wavelength)} meters", fg="#AAAAAA", bg="black")
        calculation1.pack(anchor='w', pady=(0,10))

        step2_title = tk.Label(result_frame, text="Step 2: Calculate Free Space Path Loss (FSPL)", font=bold_font, fg="#1E90FF", bg="black")
        step2_title.pack(anchor='w')
        formula2 = tk.Label(result_frame, text="Formula: FSPL(dB) = 20·log10(d) + 20·log10(f) + 32.44", fg="#2E8B57", bg="black")
        formula2.pack(anchor='w')
        params2 = tk.Label(result_frame, text=f"Parameters: d = {dist_km} km, f = {freq_mhz} MHz", bg="black")
        params2.pack(anchor='w')
        calculation2 = tk.Label(result_frame, text=f"Calculation: FSPL = 20*log10({dist_km}) + 20*log10({freq_mhz}) + 32.44 = {fspl:.2f} dB", fg="#AAAAAA", bg="black")
        calculation2.pack(anchor='w')

    except ValueError as ve:
        messagebox.showerror("Input Error", str(ve))
    except Exception as e:
        messagebox.showerror("Error", f"Unexpected error: {str(e)}")

# GUI setup
root = tk.Tk()
root.title("RF Numerical Calculator")
root.configure(bg="black")

frame = tk.Frame(root, bg="black", padx=20, pady=20)
frame.grid(row=0, column=0)

bold_font = font.Font(weight="bold", size=10)

# Labels with bold, bright color
freq_label = tk.Label(frame, text="Frequency (GHz):", fg="#00FFFF", bg="black", font=bold_font)
freq_label.grid(row=0, column=0, sticky='w', pady=5)
freq_entry = ttk.Entry(frame, width=20)
freq_entry.grid(row=0, column=1, pady=5)

dist_label = tk.Label(frame, text="Distance (km):", fg="#00FFFF", bg="black", font=bold_font)
dist_label.grid(row=1, column=0, sticky='w', pady=5)
dist_entry = ttk.Entry(frame, width=20)
dist_entry.grid(row=1, column=1, pady=5)

# Custom style for the calculate button
style = ttk.Style()
style.theme_use('clam')  # Use a theme that supports colors

style.configure("C.TButton",
                foreground="white",
                background="#FF4500",
                font=('Arial', 11, 'bold'),
                padding=6)

style.map("C.TButton",
          foreground=[('active', 'white')],
          background=[('active', '#FF6347')])  # lighter red on hover

calc_button = ttk.Button(frame, text="Calculate", style="C.TButton", command=calculate)
calc_button.grid(row=2, column=0, columnspan=2, pady=15)

result_frame = tk.Frame(frame, relief=tk.SUNKEN, borderwidth=1, width=500, height=300, bg="black")
result_frame.grid(row=3, column=0, columnspan=2, sticky='nsew')
result_frame.grid_propagate(False)

root.mainloop()
