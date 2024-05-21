import matplotlib.pyplot as plt
import pandas as pd

# Read data in csv
file_path = "class_data.csv"
data = pd.read_csv(file_path, delimiter=';')

# Create matplotlib figure
plt.style.use('ggplot')
fig, ax = plt.subplots(figsize=(14, 7))

for index, row in data.iterrows():
    ax.plot(['nº estudiantes', 'tiempo teoria', 'tiempo practica', ' tiempo ejercicios', 'nº pausas', 'puntuacion'],
            [row['estudiantes'], row['teoria'], row['practica'], row['ejercicios'], row['pausas'], row['puntuacion']],
            marker='o', label=f'{row["tipo"]}')

# Añadir leyenda, título y etiquetas
ax.legend(title='Tipo de clase')
ax.set_title('Comparación de valores por tipo de clase')
ax.set_xlabel('Categoría')
ax.set_ylabel('Valor')

# Mostrar el gráfico
plt.show()
