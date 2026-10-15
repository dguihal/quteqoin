# Recommandations d'Organisation du Code et d'Architecture pour QuteQoin

Une fois la migration technique vers Qt6 posée, ce projet (qui a débuté à l'époque de Qt4) comporte plusieurs reliquats historiques, des duplications de code et des opportunités d'optimisation de l'architecture.

Voici les recommandations détaillées pour réorganiser, moderniser et alléger l'application.

---

## 1. Nettoyage du Code Mort (Dossier `core/backend` & `Qt6::Multimedia`)

### 🚫 Le dossier `core/backend/`
Il existe une duplication presque parfaite entre deux ensembles de classes :
* **Actif** : `QQBouchot` (dans `core/`)
* **Inactif / Mort** : `QQBackend` et `QQBouchotBackend` (dans `core/backend/`)

Aucun fichier source en dehors de `core/backend/` ne fait référence à `QQBackend` ou `QQBouchotBackend`. Il s'agit probablement d'un projet de refactoring inachevé.
> [!IMPORTANT]
> Vous pouvez supprimer l'intégralité du dossier [core/backend](file:///home/damienguihal/Projects/GitHub/dguihal/quteqoin/core/backend) et retirer ses fichiers de [CMakeLists.txt](file:///home/damienguihal/Projects/GitHub/dguihal/quteqoin/CMakeLists.txt#L78-L81).

### 🚫 Dépendances multimédia inutiles
[CMakeLists.txt](file:///home/damienguihal/Projects/GitHub/dguihal/quteqoin/CMakeLists.txt#L15-L16) inclut et lie `Qt6::Multimedia` et `Qt6::MultimediaWidgets` alors que la classe `qqpinimmviewer` a été supprimée.
> [!TIP]
> Retirez ces dépendances pour accélérer la compilation et simplifier le déploiement.

---

## 2. Partage de `QNetworkAccessManager` (Réseau)

Dans [core/qqnetworkaccessor.cpp](file:///home/damienguihal/Projects/GitHub/dguihal/quteqoin/core/qqnetworkaccessor.cpp#L205-L220), chaque instance crée son propre `QNetworkAccessManager` :
```cpp
void QQNetworkAccessor::clearNetworkBackend() {
    if (! m_qnam.isNull())
        m_qnam->deleteLater();
    m_qnam = new QNetworkAccessManager(this);
    // ...
}
```
Puisque `QQBouchot` hérite de `QQNetworkAccessor`, **chaque salon de discussion dispose de son propre client HTTP indépendant**.

### 💡 Pourquoi le corriger ?
La documentation officielle de Qt spécifie qu'une seule instance de `QNetworkAccessManager` doit être utilisée pour toute l'application. Avoir plusieurs instances empêche :
1. La réutilisation des connexions TCP (HTTP Keep-Alive / HTTP/2), ce qui force à réétablir une connexion SSL/TLS à chaque rafraîchissement.
2. Le partage global du cache DNS et des cookies de manière simple.

### 🛠️ Solution proposée
Créez un service centralisé (ou une instance partagée) pour fournir un `QNetworkAccessManager` unique à tous les accesseurs réseau.

```cpp
// Exemple de manager réseau centralisé
class QQNetworkManager {
public:
    static QNetworkAccessManager* instance() {
        static QNetworkAccessManager manager;
        return &manager;
    }
};
```

---

## 3. Séparation de la responsabilité de Registre (SRP)

La classe `QQBouchot` viole le principe de responsabilité unique (Single Responsibility Principle) en représentant à la fois :
1. **Un salon (board)** individuel (sa configuration, son historique, ses requêtes).
2. **Le registre global** de tous les salons via des variables et méthodes statiques :
   ```cpp
   static QHash<QString, QQBouchot *> s_hashBouchots;
   static QList<QQBouchot *> listBouchots();
   static QList<QQBouchot *> listBouchotsGroup(const QString &groupName);
   ```

### 🛠️ Solution proposée
Extraire toute la gestion de cycle de vie et de recherche des salons dans une classe dédiée `QQBouchotManager` ou `QQBoardManager`. `QQBouchot` deviendra ainsi une classe purement représentative d'un canal de discussion.

---

## 4. Modernisation de `QQPost` (Suppression de `QObject`)

`QQPost` hérite actuellement de `QObject` (sans signaux ni slots définis) et implémente un constructeur de recopie non-standard :
```cpp
class QQPost : public QObject {
    Q_OBJECT
    // ...
    QQPost(const QQPost& post); // Recopie de QObject (anti-pattern Qt)
};
```

### 💡 Pourquoi le corriger ?
1. **Surcharge mémoire importante** : `QObject` ajoute des métadonnées lourdes à chaque message. Pour une application de chat qui charge des milliers de messages, cela consomme beaucoup de RAM inutilement.
2. **Copie impossible** : La sémantique de Qt interdit la copie des `QObject`. Vous êtes obligé d'utiliser des pointeurs `QPointer<QQPost>` partout.

### 🛠️ Solution proposée
Transformez `QQPost` en un simple **Plain Old C++ Object** (classe C++ classique sans héritage `QObject`).
* Retirez `public QObject` et la macro `Q_OBJECT`.
* Stockez directement les objets par valeur (ex: `QList<QQPost>`) au lieu de gérer des pointeurs complexes. Cela éliminera les allocations sur le tas (`new QQPost`), simplifiera l'utilisation des conteneurs Qt/C++ et évitera les fuites de mémoire.

---

## 5. Optimisation de l'utilisation des Conteneurs Qt

Dans [ui/qqpinipede.h](file:///home/damienguihal/Projects/GitHub/dguihal/quteqoin/ui/qqpinipede.h#L93), les onglets de chat sont stockés sous forme de pointeurs vers des listes :
```cpp
QHash<QString, QQListPostPtr *> m_listPostsTabMap;
```
Cela nécessite des allocations manuelles (`new QQListPostPtr()`) et des libérations manuelles verbeuses et sujettes à erreurs dans le destructeur de `QQPinipede` :
```cpp
// Code verbeux avec boucles imbriquées redondantes
QList<QString> listTabs = m_listPostsTabMap.keys();
for (const QString &tab : std::as_const(listTabs)) {
    for(int i = 0; i < listTabs.size(); i++) // boucle interne inutile
        delete m_listPostsTabMap.take(tab);
}
```

### 🛠️ Solution proposée
Utilisez le partage implicite (Copy-on-Write) de Qt en stockant la liste directement par valeur :
```cpp
QHash<QString, QQListPostPtr> m_listPostsTabMap;
```
Le nettoyage de la mémoire sera alors géré automatiquement par le destructeur du conteneur sans aucun code manuel.

---

## 6. Correction de l'Encodage Unicode dans `QuteTools`

Dans [core/qutetools.cpp](file:///home/damienguihal/Projects/GitHub/dguihal/quteqoin/core/qutetools.cpp#L64), on trouve cette ligne :
```cpp
flags.append(QStringLiteral(u"\xf0\x9f\x99\x88")); //'SEE-NO-EVIL MONKEY' (U+1F648) -> '🙈';
```
### 🐞 Le Bug
L'utilisation de préfixe `u` (qui indique une chaîne UTF-16 de type `char16_t`) combinée avec des séquences d'échappement hexadécimales représentant des octets UTF-8 crée des caractères invalides (chaque octet est promu à 16 bits individuellement).

### 🛠️ Solution
Spécifiez directement l'émoji dans le littéral ou utilisez l'échappement universel unicode (UCN) :
```cpp
flags.append(QStringLiteral(u"🙈"));
// OU
flags.append(QStringLiteral(u"\U0001F648"));
```

---

## 7. Structure des Dossiers du Projet

Pour un projet moderne, il est d'usage de vider la racine du projet des fichiers de code :
* Déplacez `main.cpp`, `mainwindow.cpp/h/ui` dans un dossier `src/` ou `src/ui/`.
* Si le support QML est abandonné au profit de QtWidgets, supprimez les fichiers inutilisés du dossier `qml/` et les directives `#ifdef ENABLE_QML` dans `main.cpp`.
