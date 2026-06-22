//===========================================================
// C#-урок: объекты чертежа — идентификация и приведение типов.
// Проект LearnMulticadNet, класс Commands. Команда LMN_CMD1.
//===========================================================

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

using Multicad;
using Multicad.AplicationServices;
using Multicad.DatabaseServices;
using Multicad.Geometry;
using Multicad.DatabaseServices.StandardObjects;

namespace LearnMulticadNet
{
    internal class Commands
    {
        // --- Вариант 1: выбор по фильтру + идентификация типа ---
        static public void SampleCmd1()
        {
            // Фильтр объектов: активный лист, только геометрические примитивы
            ObjectFilter flt = new ObjectFilter();
            flt.AddDoc(McDocumentsManager.ActiveSheet);
            flt.AddType(typeof(DbGeometry));

            Matrix3d tfmOfs = Matrix3d.Displacement(new Vector3d(100, 0, 0));

            List<McObjectId> ids = flt.GetObjects();
            foreach (var id in ids)
            {
                McDbEntity dbe = id.GetObjectOfType<McDbEntity>();

                // Проверка типа БЕЗ получения/открытия объекта
                if (id.IdentifiesObjectOfType<DbLine>())
                    dbe.Transform(tfmOfs);          // отрезки — сдвинуть
                if (id.IdentifiesObjectOfType<DbCircle>())
                    dbe.Color = Color.Red;          // окружности — покрасить
            }
        }

        // --- Вариант 2: способы приведения типов ---
        static public void SampleCmd1_Casting()
        {
            McObject   obj = null;
            McDbObject dbo = null;
            McDbEntity dbe = null;
            McEntity   ent = null;

            dbo = dbe;                                  // вверх по иерархии — неявно
            dbe = dbo as McDbEntity;                    // as: null, если не тот тип
            ent = dbo.Cast<McEntity>();                 // Cast: может бросить исключение
            ent = dbo.ID.GetObjectOfType<McEntity>();   // то, что нужно (работает и для наследников)
            if (ent != null)
            {
                // ...
            }
        }
    }
}
