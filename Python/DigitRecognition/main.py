import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers

mnist = tf.keras.datasets.mnist
(x_train, y_train), (x_test, y_test) = mnist.load_data()

x_train = x_train.astype('float32') / 255.0
x_test = x_test.astype('float32') / 255.0

x_train = np.expand_dims(x_train, -1)
x_test = np.expand_dims(x_test, -1)

def create_model():
    model = keras.Sequential([
        layers.Input(shape=(28, 28, 1)),
        layers.Conv2D(32, (3, 3), activation='relu', padding='same'),
        layers.BatchNormalization(),
        layers.Conv2D(32, (3, 3), activation='relu', padding='same'),
        layers.BatchNormalization(),
        layers.MaxPooling2D((2, 2)),
        layers.Dropout(0.25),

        layers.Conv2D(64, (3, 3), activation='relu', padding='same'),
        layers.BatchNormalization(),
        layers.Conv2D(64, (3, 3), activation='relu', padding='same'),
        layers.BatchNormalization(),
        layers.MaxPooling2D((2, 2)),
        layers.Dropout(0.25),

        layers.Flatten(),
        layers.Dense(256, activation='relu'),
        layers.BatchNormalization(),
        layers.Dropout(0.5),
        layers.Dense(10, activation='softmax')
    ])
    return model

model_path = "model_cnn.h5"

if os.path.exists(model_path):
    model = tf.keras.models.load_model(model_path)
else:
    model = create_model()
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=0.001),
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )

    callbacks = [
        keras.callbacks.EarlyStopping(patience=5, restore_best_weights=True),
        keras.callbacks.ReduceLROnPlateau(factor=0.5, patience=3, min_lr=1e-6)
    ]

    model.fit(
        x_train, y_train,
        epochs=5,
        batch_size=128,
        validation_data=(x_test, y_test),
        callbacks=callbacks
    )
    model.save(model_path)

image_number_jpg = 1
while os.path.isfile(f"digits/{image_number_jpg}.jpg"):
    try:
        img = cv2.imread(f"digits/{image_number_jpg}.jpg", cv2.IMREAD_GRAYSCALE)
        img = cv2.bitwise_not(img)
        img = cv2.resize(img, (28, 28), interpolation=cv2.INTER_AREA)
        img = img.astype('float32') / 255.0
        img = img.reshape(1, 28, 28, 1)
        prediction = model.predict(img, verbose=0)
        print(f"This digit is probably a {np.argmax(prediction)} (confidence: {np.max(prediction):.2%})")
        plt.imshow(img[0, :, :, 0], cmap=plt.cm.binary)
        plt.show()
    except Exception as e:
        print(f"Error: {e}")
    image_number_jpg += 1

loss, accuracy = model.evaluate(x_test, y_test, verbose=0)
print(f"Test Loss: {loss:.4f}")
print(f"Test Accuracy: {accuracy:.4f}")
