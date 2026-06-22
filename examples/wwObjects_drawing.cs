//===========================================================
// Практика (.NET/C#): работа с объектами чертежа через MultiCAD.NET.
// Файл wwObjects.cs (SandboxTestsCs) — команда test_wwobj.
// .NET-эквивалент C++ примера test_LVA_drawing.cpp.
//===========================================================

using Multicad.Runtime;
using Multicad.DatabaseServices;
using Multicad.DatabaseServices.StandardObjects;
using Multicad;
using Multicad.Geometry;
using Multicad.CustomObjectBase;
using Multicad.AplicationServices;
using System.Threading;
using Multicad.Objects;
using Multicad.Mc3D;

namespace SandboxTestsCs
{
    class wwObjects
    {
        [CommandMethod("test_wwobj", CommandFlags.NoCheck | CommandFlags.NoPrefix)]
        public bool test_wwObjects()
        {
            // --- 1. Дуга/окружность: радиус + цвет ---
            McObjectId idCirc = McObjectId.FromHandle(0x4BA);
            McDbEntity CircArcDBE = idCirc.GetObjectOfType<McDbEntity>();
            if (CircArcDBE != null)
            {
                DbCircle circ = idCirc.GetObjectOfType<DbCircle>();
                if (circ != null)
                    circ.Radius = 5000;

                CircArcDBE.Color = System.Drawing.Color.Green;
                CircArcDBE.Update();
            }

            // --- 2. Отрезок: сдвиг матрицей ---
            McObjectId idLine = McObjectId.FromHandle(0x4B8);
            McDbEntity LineDBE = idLine.GetObjectOfType<McDbEntity>();
            if (LineDBE != null)
            {
                Matrix3d tfmShifting = Matrix3d.Displacement(new Vector3d(1000, 0, 0));
                LineDBE.Transform(tfmShifting);
                LineDBE.Update();
            }

            // --- 3. Второй отрезок: удалить ---
            McObjectId idLine2 = McObjectId.FromHandle(0x504);
            McDbEntity LineDBE2 = idLine2.GetObjectOfType<McDbEntity>();
            if (LineDBE2 != null)
                LineDBE2.Erase();

            return true;
        }
    }
}
