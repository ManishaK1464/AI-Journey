import tkinter as tk
from tkinter import ttk, messagebox, font
import math
from PIL import Image, ImageTk  # Pillow library, install via pip if needed

# Global list to hold image references and prevent GC
image_refs = []

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

def format_number(num, unit=None):
    if abs(num) >= 1e5 or abs(num) < 1e-3:
        val = sci_notation_superscript(num)
    else:
        val = f"{num:,.4f}"
    return f"{val}" + (f" {unit}" if unit else "")

def fresnel_radius(wavelength, d1_m, d2_m, total_d_m):
    return 0.5 * math.sqrt(wavelength * d1_m * d2_m / total_d_m)

def log10(x):
    return math.log10(x) if x > 0 else float('-inf')

recipes = {
    "Recipe 1: 10 GHz Point-to-Point Backhaul": {
        "freq_ghz": 10,
        "distance_km": 8,
        "P_tx_dBm": 30,
        "G_tx_dBi": 30,
        "G_rx_dBi": 30,
        "L_tx_dB": 2,
        "L_rx_dB": 2,
        "L_misc_dB": 6,
        "receiver_sensitivity_dBm": -85,
        "link_margin_required_dB": 20,
        "notes": (
            "Goal: Connect two towers 8 km apart for high-capacity data link.\n"
            "High-gain dish antennas approx 0.9 m diameter.\n"
            "Ensure Fresnel zone clearance >80%.\n"
            "Includes cable losses and misc losses.\n"
            "Amplifier fT ≥ 50 GHz, fmax ≥ 60 GHz.\n"
            "Rain attenuation moderate, included in misc losses."
        )
    },
    "Recipe 2: 28 GHz 5G Small-Cell Base Station": {
        "freq_ghz": 28,
        "distance_km": 1,
        "P_tx_dBm": 23,
        "G_tx_dBi": 18,
        "G_rx_dBi": 0,
        "L_tx_dB": 1,
        "L_rx_dB": 1,
        "L_misc_dB": 10,
        "receiver_sensitivity_dBm": -100,
        "link_margin_required_dB": None,
        "notes": (
            "Goal: Cover a 1 km radius urban area with 28 GHz 5G sector antenna.\n"
            "Microstrip patch antenna element ~0.535 cm.\n"
            "Urban microcell path loss model used.\n"
            "Includes blockage and fading losses.\n"
            "Power amplifiers require fT > 150 GHz, fmax > 200 GHz.\n"
            "Heavy rain attenuation ~10 dB/km included in misc losses."
        )
    }
}

def on_recipe_change(event):
    recipe_name = recipe_combo.get()
    params = recipes[recipe_name]

    freq_entry.delete(0, tk.END)
    freq_entry.insert(0, str(params["freq_ghz"]))

    dist_entry.delete(0, tk.END)
    dist_entry.insert(0, str(params["distance_km"]))

    status_label.config(text=f"Loaded {recipe_name} with default parameters.", fg="#00FF00")

def calculate():
    try:
        recipe_name = recipe_combo.get()
        params = recipes[recipe_name]

        try:
            freq_ghz = float(freq_entry.get())
            freq_defaulted = False
        except:
            freq_ghz = params["freq_ghz"]
            freq_defaulted = True

        try:
            dist_km = float(dist_entry.get())
            dist_defaulted = False
        except:
            dist_km = params["distance_km"]
            dist_defaulted = True

        if freq_ghz <= 0 or dist_km <= 0:
            raise ValueError("Frequency and distance must be positive numbers.")

        P_tx = params["P_tx_dBm"]
        G_tx = params["G_tx_dBi"]
        G_rx = params["G_rx_dBi"]
        L_tx = params["L_tx_dB"]
        L_rx = params["L_rx_dB"]
        L_misc = params["L_misc_dB"]
        Rx_sens = params["receiver_sensitivity_dBm"]
        margin_req = params["link_margin_required_dB"]
        notes = params["notes"]

        freq_hz = freq_ghz * 1e9
        dist_m = dist_km * 1000

        wavelength = c / freq_hz

        half_wave_dipole = wavelength / 2
        quarter_wave_monopole = wavelength / 4

        dish_diameter = 30 * wavelength if recipe_name == "Recipe 1: 10 GHz Point-to-Point Backhaul" else None
        waveguide_width = wavelength / 2 if recipe_name == "Recipe 1: 10 GHz Point-to-Point Backhaul" else None

        d1 = d2 = dist_m / 2
        fresnel_r = fresnel_radius(wavelength, d1, d2, dist_m)
        fresnel_clearance = 0.8 * fresnel_r

        freq_mhz = freq_ghz * 1e3
        fspl = 32.44 + 20 * log10(freq_mhz) + 20 * log10(dist_km)

        P_rx = P_tx + G_tx - L_tx - fspl - L_misc + G_rx - L_rx
        link_margin = P_rx - Rx_sens

        # Clear previous results
        for w in scrollable_frame.winfo_children():
            w.destroy()

        def add_label(text, fg="#FFFFFF", pady=(0,0), font_style=None):
            font_style = font_style or normal_font
            tk.Label(scrollable_frame, text=text, fg=fg, bg="black", font=font_style, justify="left", wraplength=win_width-100).pack(anchor="w", pady=pady)

        tk.Label(scrollable_frame, text=f"{recipe_name}", fg="#FFD700", bg="black", font=title_font).pack(anchor="w", pady=(0,5))
        tk.Label(scrollable_frame, text="Notes:", fg="#87CEEB", bg="black", font=bold_font).pack(anchor="w")
        tk.Label(scrollable_frame, text=notes, fg="#AAAAAA", bg="black", justify="left", wraplength=win_width-100).pack(anchor="w", pady=(0,10))

        add_label(f"Operating frequency f = {format_number(freq_hz, 'Hz')}" + (" (default from recipe)" if freq_defaulted else " (user input)"), fg="#00FFFF", font_style=bold_font)
        add_label(f"Distance d = {format_number(dist_m, 'm')}" + (" (default from recipe)" if dist_defaulted else " (user input)"), fg="#00FFFF", font_style=bold_font)
        add_label(f"Speed of light c = {sci_notation_superscript(c)} m/s", fg="#00FF00")

        add_label("\nSTEP 1: Calculate Wavelength (λ):", fg="#1E90FF", font_style=bold_font)
        add_label("Formula: λ = c / f", fg="#32CD32")
        add_label(f"Calculation: λ = {sci_notation_superscript(c)} / {format_number(freq_hz)} = {format_number(wavelength, 'm')}", fg="#CCCCCC")

        add_label("\nSTEP 2: Antenna & Waveguide Sizing:", fg="#1E90FF", font_style=bold_font)
        add_label(f"Half-wave dipole length = λ / 2 = {format_number(half_wave_dipole, 'm')}")
        add_label(f"Quarter-wave monopole length = λ / 4 = {format_number(quarter_wave_monopole, 'm')}")

        if dish_diameter:
            add_label(f"Dish diameter ≈ 30 × λ = {format_number(dish_diameter, 'm')}")
            add_label(f"Waveguide internal width ≈ λ / 2 = {format_number(waveguide_width, 'm')}")
        else:
            add_label(f"Microstrip patch antenna element size ≈ λ / 2 = {format_number(half_wave_dipole, 'm')}")
            add_label("Array design: 16×16 elements, element spacing = λ / 2")

        add_label("\nSTEP 3: Fresnel Zone Check:", fg="#1E90FF", font_style=bold_font)
        add_label(f"First Fresnel zone radius at mid-path r = 0.5·√(λ·d₁·d₂/d) where d₁ = d₂ = {format_number(d1, 'm')}")
        add_label(f"Calculated Fresnel radius r ≈ {format_number(fresnel_r, 'm')}")
        add_label(f"Recommended clearance > 80% of r ≈ {format_number(fresnel_clearance, 'm')} above obstacles")

        add_label("\nSTEP 4: Free-Space Path Loss (FSPL):", fg="#1E90FF", font_style=bold_font)
        add_label("Formula: FSPL(dB) = 32.44 + 20·log₁₀(f_MHz) + 20·log₁₀(d_km)", fg="#32CD32")
        add_label(f"f_MHz = {format_number(freq_mhz, 'MHz')}, d_km = {dist_km}")
        add_label(f"Calculation: FSPL = 32.44 + 20·log₁₀({format_number(freq_mhz)}) + 20·log₁₀({dist_km}) = {fspl:.2f} dB")

        add_label("\nSTEP 5: Link Budget:", fg="#1E90FF", font_style=bold_font)
        add_label(f"Transmit power P_tx = {P_tx} dBm")
        add_label(f"Tx antenna gain G_tx = {G_tx} dBi")
        add_label(f"Rx antenna gain G_rx = {G_rx} dBi")
        add_label(f"Cable losses L_tx = {L_tx} dB, L_rx = {L_rx} dB")
        add_label(f"Misc losses L_misc = {L_misc} dB")
        add_label(f"Received power P_rx = P_tx + G_tx – L_tx – FSPL – L_misc + G_rx – L_rx")
        add_label(f"Calculation: {P_tx} + {G_tx} – {L_tx} – {fspl:.2f} – {L_misc} + {G_rx} – {L_rx} = {P_rx:.2f} dBm")

        add_label(f"Receiver sensitivity = {Rx_sens} dBm")
        add_label(f"Link margin = P_rx – Rx sensitivity = {link_margin:.2f} dB")
        if margin_req is not None:
            add_label(f"Required margin > {margin_req} dB → {'✅ Pass' if link_margin >= margin_req else '❌ Fail'}", fg="#00FF00" if link_margin >= margin_req else "#FF4500")
        else:
            add_label(f"Margin requirement not specified for this recipe.")

        if recipe_name == "Recipe 1: 10 GHz Point-to-Point Backhaul":
            add_label("\nTRANSISTOR & AMPLIFIER CHECK:", fg="#1E90FF", font_style=bold_font)
            add_label("Amplifier fT ≥ 50 GHz, fmax ≥ 60 GHz for stable gain and oscillation check")
            add_label("Rain attenuation moderate (~2-4 dB/km heavy rain) included in misc losses")

        elif recipe_name == "Recipe 2: 28 GHz 5G Small-Cell Base Station":
            add_label("\nTRANSISTOR & AMPLIFIER CHECK:", fg="#1E90FF", font_style=bold_font)
            add_label("Power amplifiers require transistors with fT > 150 GHz, fmax > 200 GHz")
            add_label("Rain attenuation high (~10 dB/km heavy rain), included in misc losses")

        # Display images
        try:
            antenna_img = Image.open(r"C:\CodeEmailGenerator\RF\antenna.jpeg")
            antenna_img = antenna_img.resize((220, 140), Image.ANTIALIAS)
            antenna_photo = ImageTk.PhotoImage(antenna_img)
            image_refs.append(antenna_photo)  # keep reference

            antenna_label = tk.Label(scrollable_frame, image=antenna_photo, bg="black")
            antenna_label.pack(side="left", padx=10, pady=10)

            fresnel_img = Image.open(r"C:\CodeEmailGenerator\RF\fresnel.jpeg")
            fresnel_img = fresnel_img.resize((220, 140), Image.ANTIALIAS)
            fresnel_photo = ImageTk.PhotoImage(fresnel_img)
            image_refs.append(fresnel_photo)  # keep reference

            fresnel_label = tk.Label(scrollable_frame, image=fresnel_photo, bg="black")
            fresnel_label.pack(side="left", padx=10, pady=10)
        except Exception as e:
            add_label(f"(Image files not found or error loading images: {e})", fg="#FF5555")

    except ValueError as ve:
        messagebox.showerror("Input Error", str(ve))
    except Exception as e:
        messagebox.showerror("Error", f"Unexpected error: {str(e)}")

# GUI setup
root = tk.Tk()
root.title("RF Numerical Calculator - Recipes")
root.configure(bg="black")

screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()

win_width = int(screen_width * 0.9)
win_height = int(screen_height * 0.9)

root.geometry(f"{win_width}x{win_height}")
root.minsize(600, 500)
root.maxsize(screen_width, screen_height)

frame = tk.Frame(root, bg="black", padx=20, pady=20)
frame.grid(row=0, column=0, sticky='nsew')

# Configure grid weight for resizing
root.grid_rowconfigure(0, weight=1)
root.grid_columnconfigure(0, weight=1)
frame.grid_rowconfigure(5, weight=1)
frame.grid_columnconfigure(0, weight=1)

bold_font = font.Font(weight="bold", size=10)
normal_font = font.Font(size=9)
title_font = font.Font(weight="bold", size=12)

tk.Label(frame, text="Select Recipe:", fg="#00FFFF", bg="black", font=bold_font).grid(row=0, column=0, sticky='w', pady=5)
recipe_combo = ttk.Combobox(frame, values=list(recipes.keys()), state="readonly", width=45)
recipe_combo.grid(row=0, column=1, pady=5)
recipe_combo.current(0)
recipe_combo.bind("<<ComboboxSelected>>", on_recipe_change)

freq_label = tk.Label(frame, text="Frequency (GHz):", fg="#00FFFF", bg="black", font=bold_font)
freq_label.grid(row=1, column=0, sticky='w', pady=5)
freq_entry = ttk.Entry(frame, width=20)
freq_entry.grid(row=1, column=1, pady=5)

dist_label = tk.Label(frame, text="Distance (km):", fg="#00FFFF", bg="black", font=bold_font)
dist_label.grid(row=2, column=0, sticky='w', pady=5)
dist_entry = ttk.Entry(frame, width=20)
dist_entry.grid(row=2, column=1, pady=5)

style = ttk.Style()
style.theme_use('clam')
style.configure("C.TButton",
                foreground="white",
                background="#FF4500",
                font=('Arial', 11, 'bold'),
                padding=6)
style.map("C.TButton",
          foreground=[('active', 'white')],
          background=[('active', '#FF6347')])

calc_button = ttk.Button(frame, text="Calculate", style="C.TButton", command=calculate)
calc_button.grid(row=3, column=0, columnspan=2, pady=15)

status_label = tk.Label(frame, text="", fg="#00FF00", bg="black", font=normal_font)
status_label.grid(row=4, column=0, columnspan=2)

# Scrollable frame for results
canvas = tk.Canvas(frame, bg="black", highlightthickness=0)
scrollbar = ttk.Scrollbar(frame, orient="vertical", command=canvas.yview)
scrollable_frame = tk.Frame(canvas, bg="black")

scrollable_frame.bind(
    "<Configure>",
    lambda e: canvas.configure(
        scrollregion=canvas.bbox("all")
    )
)

canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
canvas.configure(yscrollcommand=scrollbar.set)

canvas.grid(row=5, column=0, sticky='nsew')
scrollbar.grid(row=5, column=1, sticky='ns')

frame.grid_rowconfigure(5, weight=1)
frame.grid_columnconfigure(0, weight=1)

on_recipe_change(None)

root.mainloop()
