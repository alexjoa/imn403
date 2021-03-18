TP2 IMN403

Alexandre Joanisse 	17094467
Pierre-Emmanuel Goffi	18110928

Le fichier opengl dans le directory de la solution devrait assurer que les bonne version des libraries opengl soient utilisées.
Si jamais ca ne fonctionne pas un video de demonstration demo.mp4 du programme sera inclu dans le dossier de solution.
Le code du TP2 du cours d'infographie a été utilisé pour débuter. http://info.usherbrooke.ca/pmjodoin/cours/IMN428/index.html

1. Échantillonnage modifiable par l’utilisateur à l’aide des touches + et - du clavier. Les pas d'échantillonnage possible sont 
limitées à 0.01, 0.05, 0.1, 0.2, 0.4, 1.0, 2.0, 5.0, 10.0 pour permettre la division en triangles dans openGL. Le pas d'échantillonnage
initial est 0.1. Lorsque cette valeur est modifiée sa nouvelle valeur est affiché dans la console.

2. Les 5 colormaps ont été implentées. Pour changer de colormap appuyez sur la touche c ou C. La colormap initiale est 'rainbow'.

3. Appuyez sur la touche d ou D pour activer ou désactiver la carte de hauteurs. Par défault est activée.

4. Initialment le mode (auto ou manuel) et le nombre d'isocontours générés est choisit par l'utilisateur. 
Automatique: le nombre d'isocontours sont distribué de façon uniforme à la même distance un de l'autre.
Manuel: l'utilisateur entre les valeurs scalaires désirés.
Appuyez sur la touche a ou A change le mode (auto ou manuel).
Appuyez sur la touche x ou X augmente le nombre d'isocontours de 1, jusqu'à un maximum de 20 isocontours.
Appuyez sur la touche z ou Z réduit le nombre d'isocontours de 1. Possibilité de réduire à 0 isocontours.
Appuyez sur la touche s ou S change l'isocontour actuellement modifiable. (voir contrôles souris)
Tous ces changements sont affiché dans la console.


Contrôles souris:
-----------------
clic-gauche : Modifie selon l'option choisie dans le menu (clic-droit).
clic-droit : Affiche le menu d'option pour le clic-gauche.
clic-milieu (clic de la roulette): Zoom.

Menu(clic-droit):
Deplace camera, Permet de déplacer la camera autour du plan ou carte de hauteurs.
Deplace isocontours, Permet de modifier la valeur v de l'isocontour actuel.	IMPORTANT
Retour a l'origine, retourne la camera à l'origine.
Quitter


