#ifndef QQBOUCHOTDEF_H
#define QQBOUCHOTDEF_H

#define DEFAULT_REFRESH_RATE 30

//Définition des bouchots préconfigurés
// tiré d'olcc by Chrisix




#define BOUCHOTDEF_SIZE 3
static bouchotDefStruct bouchotsDef[] =
{
	{ "dlfp", "http://linuxfr.org/board/index.xml", "http://linuxfr.org/board", "board[message]=%m",
	  "", "linuxfr,beyrouth,passite,dapassite", "_linuxfr.org_session", SlipTagsEncoded },
	{ "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "",
	  "#ffccaa", "llg", "", SlipTagsRaw },
	{ "euromussels", "http://euromussels.eu/?q=tribune.xml", "http://euromussels.eu/?q=tribune/post", "",
	  "#d0d0ff", "euro,euroxers", "", SlipTagsEncoded }
};
#endif // QQBOUCHOTDEF_H
