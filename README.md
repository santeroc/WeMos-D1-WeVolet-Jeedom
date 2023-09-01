# WeMos-D1-WeVolet-Jeedom
Module permettant de remplacer un bouton manuel à trois positions par un module embarquant un serveur web interfaçable à une plateforme domotique Jeedom.

## Description détaillée

Ce projet a pour ambition de remplacer un bouton de type inverseur filaire par un module embarquant un serveur web permettant de gérer la montée, la descente et l'arrêt d'un volet roulant électrique. Une fois installé il est possible de commander un volet roulant électrique à l'aide d'un navigateur web ou d'un script sur une plateforme domotique comme Jeedom. La carte ARCELI WeMos D1 R2, à la base de ce module, embarque une architecture ESP8266 et une carte WIFI intégrée.

## Recommandation
Il est vivement recommandé d'avoir de bonnes connaissances en électricité générale avant de réaliser ou de manipuler tout câblage électrique.
Ceci afin d'appréhender au minimum les risques d'électrocution.

## Structure du projet
- **Documentation\\** : Documentation du projet à lire avant de commencer
	- ``WeMos-D1-WeVolet-Jeedom.pdf`` : Document de base nécessaire pour fabriquer le projet et utiliser le code source ci-dessous.
- **Source\\**        : Code source du projet 
	-  ``WebServer_Volet_PressBN.ino`` : Code source Arduino à adapter selon la documentaiton ci-dessus.
	
## Version
**Dernière version stable :** 1.0

## Auteur
* **Claude Santero** _alias_ [@santeroc](https://github.com/santeroc)

## Licence

Ce projet est sous licence ``GPLv3`` - voir le fichier [LICENSE.md](LICENSE.md) pour plus d'informations.
