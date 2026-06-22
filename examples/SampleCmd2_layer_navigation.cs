//===========================================================
// C#-урок: навигация между уровнями объектной модели MultiCAD.
// McDbObject / McDbEntity (уровень DWG) <-> McEntity (уровень MultiCAD).
//===========================================================

static public void SampleCmd2()
{
    McDbObject pDBO = null;
    McDbEntity pDBE = null;
    McEntity   pEnt = null;

    // По ID получить объект MultiCAD (McEntity)
    pEnt = pDBO.ID.GetObjectOfType<McEntity>();

    // McDbEntity -> McEntity (родной объект MultiCAD)
    pEnt = pDBE.Entity;

    // McEntity -> McDbEntity (обёртка уровня DWG)
    pDBE = pEnt.DbEntity;
}
