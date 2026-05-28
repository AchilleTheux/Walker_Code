#!/usr/bin/env python3
import sympy as sp

# ----------------------------
# Symbols
# ----------------------------
t1, t2 = sp.symbols("t1 t2", real=True)
px, py = sp.symbols("px py", real=True)
fx, fy = sp.symbols("fx fy", real=True)

# ----------------------------
# Geometry, in mm
# ----------------------------
d = sp.Rational(448, 10)  # 44.8
a = sp.Integer(88)
b = sp.Integer(108)
e = sp.Integer(38)

# ----------------------------
# Joint positions
# ----------------------------
Cx = a * sp.cos(t1)
Cy = d / 2 + a * sp.sin(t1)

Dx = a * sp.cos(t2)
Dy = -d / 2 + a * sp.sin(t2)

# ----------------------------
# Original equations
# ----------------------------
eq1 = (px - Cx)**2 + (py - Cy)**2 - b**2
eq2 = (px - Dx)**2 + (py - Dy)**2 - b**2

eq3 = fx - (px + e * (px - Cx) / b)
eq4 = fy - (py + e * (py - Cy) / b)

eqs = [eq1, eq2, eq3, eq4]

print("\nOriginal equations:")
for i, eq in enumerate(eqs, start=1):
    print(f"\neq{i} =")
    print(sp.factor(eq))

# ----------------------------
# Solve symbolically for px, py
# from foot equations
# ----------------------------
sol_p = sp.solve([eq3, eq4], [px, py], dict=True)[0]

px_expr = sp.simplify(sol_p[px])
py_expr = sp.simplify(sol_p[py])

print("\n\npx expression:")
print(px_expr)

print("\npy expression:")
print(py_expr)

# ----------------------------
# Substitute px, py into linkage constraints
# This gives equations only in:
# t1, t2, fx, fy
# ----------------------------
ik_eq1 = sp.simplify(eq1.subs({px: px_expr, py: py_expr}))
ik_eq2 = sp.simplify(eq2.subs({px: px_expr, py: py_expr}))

ik_eq1 = sp.factor(ik_eq1)
ik_eq2 = sp.factor(ik_eq2)

print("\n\nReduced inverse-kinematics equations:")
print("\nik_eq1(t1, fx, fy) =")
print(ik_eq1)

print("\nik_eq2(t1, t2, fx, fy) =")
print(ik_eq2)

# ----------------------------
# Optional: produce cleaner expanded forms
# ----------------------------
print("\n\nExpanded reduced equations:")
print("\nik_eq1 expanded =")
print(sp.expand(ik_eq1))

print("\nik_eq2 expanded =")
print(sp.expand(ik_eq2))

# ----------------------------
# Optional: generate Python-callable functions
# Useful if you later want numerical solving
# ----------------------------
ik_eq1_func = sp.lambdify((t1, fx, fy), ik_eq1, "numpy")
ik_eq2_func = sp.lambdify((t1, t2, fx, fy), ik_eq2, "numpy")

print("\n\nDone.")
