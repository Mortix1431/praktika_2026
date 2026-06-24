//===========================================================
// Практика (C#): создание листового тела (sheet metal) через MultiCAD.NET.
// Файл SimpleSheeSolidSample.cs — команда sb_cs_sheetsolid.
//===========================================================

namespace SandboxCS
{
    [ContainsCommands]
    class SimpleSheeSolidSample
    {
        [CommandMethod("sb_cs_sheetsolid", CommandFlags.NoCheck | CommandFlags.NoPrefix)]
        public static void CreateSheetSolid()
        {
            // Создаём фичу листового тела и добавляем его в документ.
            ISmSolid   smSolid = Mc3dSolid.SmCreateSheetSolid();
            Mc3dSolid  solid   = smSolid.Solid;
            McObjectManager.Add2Document(solid.DbEntity);

            // Создаём эскиз с прямоугольным контуром внутри.
            PlanarSketch sketch = solid.AddPlanarSketch();
            sketch.DbEntity.Visibility = 0;
            McObjectManager.Add2Document(sketch.DbEntity);

            PolylineId rect = PolylineId.CreateRectangle(new Point3d(0, 0, 0),
                                                         new Point3d(300, 200, 0));
            sketch.AddGeometry(McObjectId.NewID(), new EntityGeometry(rect));

            // Создаём эскизный профиль из геометрии эскиза (внешний контур).
            SketchProfile prof = sketch.CreateProfile();
            McObjectManager.Add2Document(prof.DbEntity);
            prof.DbEntity.Visibility = 0;
            prof.AutoProcessExternalContours();

            // Связываем профиль с листовым телом.
            smSolid.ProfileID     = prof.ID;
            smSolid.ExtrusionType = SheetSolidExtrusionType.Both;
            McObjectManager.UpdateAll();

            // Устанавливаем режим выдавливания и расстояния через связанные переменные.
            DocumentVariablesManager dvm =
                solid.DbEntity.Document.GetDocumentVariablesManager(Constraint2dNetType.MCS);
            // толщина листа в сторону "forward"
            dvm.SetVariableExpression(smSolid.Dist1VarID, "2");
            // толщина листа в сторону "reverse"
            dvm.SetVariableExpression(smSolid.Dist2VarID, "3");

            McObjectManager.UpdateAll();
        }
    }
}
