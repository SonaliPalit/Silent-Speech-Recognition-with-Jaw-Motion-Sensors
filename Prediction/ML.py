import os
import pandas as pd
import numpy as np
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.decomposition import PCA
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import classification_report, accuracy_score, confusion_matrix
import matplotlib.pyplot as plt
import seaborn as sns
import time
import joblib

def load_data(label_df, data_dir):
    features = []
    labels = []

    for _, row in label_df.iterrows():
        filename = os.path.join(data_dir, row['filename'] + ".txt")
        # try:
        if not os.path.exists(filename):
            print(f"File not found: {filename}")
            continue
        df = pd.read_csv(filename)
        df.columns = df.columns.str.strip()
        required_columns = ['Acce_X', 'Acce_Y', 'Acce_Z', 'Gyro_X', 'Gyro_Y', 'Gyro_Z']
        # if not all(col in df.columns for col in required_columns):
        #     raise ValueError(f"Missing required columns in file {filename}")
        data = df[required_columns].values.astype(np.float32)
        data_min = data.min(axis=0)
        data_max = data.max(axis=0)
        data_range = data_max - data_min
        data_range[data_range == 0] = 1   
        data = (data - data_min) / data_range

        timesteps = 400
        if data.shape[0] > timesteps:
            data = data[:timesteps, :]
        elif data.shape[0] < timesteps:
            padding = np.zeros((timesteps - data.shape[0], data.shape[1]))
            data = np.vstack((data, padding))

        features.append(data.flatten())
        labels.append(row['label'])

        # except Exception as e:
        #     print(f"Error processing file {filename}: {e}")

    features = np.array(features)
    labels = np.array(labels)
    features = np.nan_to_num(features)
    return features, labels

#
def train_and_evaluate_model(model, X_train, y_train, X_test, y_test, model_name):
    model.fit(X_train, y_train)
    y_pred = model.predict(X_test)

    accuracy = accuracy_score(y_test, y_pred)
    print(f"{model_name} Accuracy: {accuracy:.3%}")
    print(classification_report(y_test, y_pred))

    conf_matrix = confusion_matrix(y_test, y_pred)
    sns.heatmap(conf_matrix, annot=True, cmap="Blues", xticklabels=np.unique(y_test), yticklabels=np.unique(y_test))
    plt.title(f"{model_name} Confusion Matrix")
    plt.xlabel("Predicted")
    plt.ylabel("Actual")
    plt.show()

if __name__ == "__main__":
    train_labels = pd.read_csv("train_data.csv")
    val_labels = pd.read_csv("val_data.csv")
    train_dir = val_dir = "help-words"

    X_train, y_train = load_data(train_labels, train_dir)  
    X_test, y_test = load_data(val_labels, val_dir)

    print(f"NaN in X_train: {np.isnan(X_train).sum()}")
    print(f"NaN in X_test: {np.isnan(X_test).sum()}")

    models = {
        "SVM": SVC(kernel='rbf', probability=True),
        "Random Forest": RandomForestClassifier(n_estimators=100, random_state=42),
        "kNN": KNeighborsClassifier(n_neighbors=25),
        "MLP Neural Network": MLPClassifier(hidden_layer_sizes=(128, 64), max_iter=500, random_state=42)
    }

    for model_name, model in models.items():
        print(f"Training and evaluating {model_name}...")
        start_time = time.time()
        train_and_evaluate_model(model, X_train, y_train, X_test, y_test, model_name)
        print(f"{model_name} completed in {time.time() - start_time:.2f} seconds")

    #hyperparameter tuning for MLP nueral network
    param_grid = {
        'hidden_layer_sizes': [(128, 64), (256, 128), (128, 128, 64)],
        'activation': ['relu', 'tanh'],
        'solver': ['adam', 'sgd'],
        'learning_rate_init': [0.001, 0.01],
        'max_iter': [500]
    }
    grid = GridSearchCV(MLPClassifier(random_state=42), param_grid, cv=3, verbose=3)
    grid.fit(X_train, y_train)
    print(f"Best Parameters for MLP: {grid.best_params_}")
    best_mlp = grid.best_estimator_

    model_filename = "model.pkl"
    joblib.dump(best_mlp, model_filename)
    print(f"model saved in {model_filename}")
    print("Best MLP model...")
    train_and_evaluate_model(best_mlp, X_train, y_train, X_test, y_test, "Tuned MLP Neural Network")